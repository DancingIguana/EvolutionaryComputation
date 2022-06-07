import PySimpleGUI as sg

from PIL import Image, ImageDraw
from sympy import E
import cv2
from skimage.metrics import mean_squared_error
import numpy as np
import copy
#from IPython import display
import random
import math
from io import BytesIO

"""
GENETIC ALGORITHM SECTION
"""

def toCv2(img):
  return cv2.cvtColor(np.array(img),cv2.COLOR_RGB2BGR)


"""
POLYGON CLASS
Purpose: store coordinates and colors of a triangle
"""
class Polygon():
  def __init__(self, vertices = [], colors = [], nVertices = 0):
    self.vertices = vertices #[(x0,y0),...,(xn,yn)]
    self.colors = colors #(R,G,B,A)
    self.nVertices = len(vertices)
  
  def __repr__(self):
    return f"Vertices = {self.vertices}\nColors = {self.colors}"

"""
CHROMOSOME CLASS
Purpose: store an image represented as a set of triangles
"""

class Chromosome():
  def __init__(self, polygons = [], aptitude = 0, width = 100, height = 100):
    self.polygons = polygons
    self.aptitude = aptitude
    self.MSE = 0
    self.width = width
    self.height = height

  def copy(self):
    polygonsC = self.polygons.copy()
    aptitudeC = self.aptitude
    widthC = self.width
    heightC = self.height
    return Chromosome(polygons = polygonsC, aptitude = aptitudeC, width = widthC, height = heightC)

  def generateImage(self):
    image = Image.new('RGB',(self.width,self.height), (0,0,0,255))
    draw = ImageDraw.Draw(image,'RGBA')

    for polygon in self.polygons:
      draw.polygon(polygon.vertices,tuple(polygon.colors))

    del draw
    return image

  def calculateAptitude(self,refImgCv2):
    self.MSE = mean_squared_error(toCv2(self.generateImage()), refImgCv2)
    self.aptitude = 1/(self.MSE/10000 + 1)


"""
POPULATION CLASS
Purpose: store the population of images/chromosomes and have
the following operators:
    - cross
    - mutation
    - parent selection
    - survivor selection
"""
class Population():
  def __init__(self,chromosomes = [], mu = 0, refImgCv2 = None, height = 100, width = 100):
    self.chromosomes = chromosomes
    self.mu = mu
    self.refImgCv2 = refImgCv2
    self.height = height
    self.width = width

  def __len__(self):
    return len(self.chromosomes)

  def __getitem__(self,i):
    return self.chromosomes[i]

  #Method 1 of mutation: mutate by random perturbations over the polygons
  def mutate1(self,i, pMut, vertexP, colorP, vertexPerturbation, colorPerturbation, transparencyPerturbation): #
    #for i in range(len(self.chromosomes)): # for each chromosome i
    for j in range(len(self.chromosomes[i].polygons)): #for each polygon in the chromosome

      if(np.random.uniform(0,1) >= pMut): continue

      for vertex in range(3):#for each vertex in the polygons

        #Mutate a vertex in the polygon
        if(np.random.uniform(0,1) < vertexP):
          point = self.chromosomes[i].polygons[j].vertices[vertex] 
          px = np.random.uniform(-vertexPerturbation,vertexPerturbation) #perturbation in x
          py = np.random.uniform(-vertexPerturbation,vertexPerturbation) #perturbation in y
          mutX = point[0] + px
          mutY = point[1] + py

          #Edge cases
          if(mutX < 0 or mutX > self.width):
            mutX = np.random.uniform(0,self.width)

          if(mutY < 0 or mutY > self.height):
            mutY = np.random.uniform(0,self.height)

          #Perform mutation in-place
          self.chromosomes[i].polygons[j].vertices[vertex] = (mutX,mutY) 

      #For R,G,B,A
      for color in range(4): 

        #Mutate color value in-place
        if(np.random.uniform(0,1) < colorP):
          c = self.chromosomes[i].polygons[j].colors[color]
          if(color < 3):
            c += np.random.randint(-colorPerturbation,colorPerturbation)
          else:
            c += np.random.randint(-transparencyPerturbation, transparencyPerturbation)

          #Edge cases
          if(c < 0 or c > 255):
            c = np.random.randint(0,255)

          self.chromosomes[i].polygons[j].colors[color] = c

    #Redefine aptitude of the new Image after mutation
    self.chromosomes[i].calculateAptitude(self.refImgCv2)

  #Method 2 of mutation: mutate by assigning completely random coordinate and color values
  def mutate2(self,i,pMut):
    #for i in range(len(self.chromosomes)):
    for j in range(len(self.chromosomes[i].polygons)):

      if(np.random.uniform(0,1) >= pMut): continue

      for vertex in range(3):
        if(np.random.uniform(0,1) < 0.33):
          self.chromosomes[i].polygons[j].vertices[vertex] = (np.random.uniform(0,self.width),np.random.uniform(0,self.height))
      
      for color in range(4):
        if(np.random.uniform(0,1) < 0.25):
          self.chromosomes[i].polygons[j].colors[color] = np.random.randint(0,255)

    self.chromosomes[i].calculateAptitude(self.refImgCv2)  

  def mutate(self,vertexP, colorP, vertexPerturbation, colorPerturbation, transparencyPerturbation, secondP, pMut):
    for i in range(len(self.chromosomes)):
      if(np.random.uniform(0,1) >= secondP):
        self.mutate1(i,pMut,vertexP, colorP, vertexPerturbation, colorPerturbation, transparencyPerturbation)
      else:
        self.mutate2(i,pMut)

  def crossover(self,p1,p2,m):
    n = len(p1.polygons)
    crossPoints = random.sample([i for i in range(1,n-1)], m)
    crossPoints.append(n)
    crossPoints.sort()
    initial = 0
    son1Polygons = []
    son2Polygons = []

    for i,point in enumerate(crossPoints):
      if(i % 2):
        son1Polygons += copy.deepcopy(p1.polygons[initial:point])
        son2Polygons += copy.deepcopy(p2.polygons[initial:point])
      else:
        son1Polygons += copy.deepcopy(p2.polygons[initial:point])
        son2Polygons += copy.deepcopy(p1.polygons[initial:point])
      
      initial = point
      
    son1 = Chromosome(son1Polygons, width = self.width, height = self.height)
    son1.calculateAptitude(self.refImgCv2)

    son2 = Chromosome(son2Polygons, width = self.width, height = self.height)
    son2.calculateAptitude(self.refImgCv2)

    return son1, son2
  
  def crossoverrand(self,p1,p2):
    son1Polygons = [random.choice([p1.polygons[i],p2.polygons[i]]) for i in range(len(p1.polygons))]
    son2Polygons = []
    for i in range(len(son1Polygons)):
      if i == p1.polygons[i]: 
        son2Polygons.append(p2.polygons[i])
      else: 
        son2Polygons.append(p1.polygons[i])


    son2Polygons = [random.choice([p1.polygons[i],p2.polygons[i]]) for i in range(len(p1.polygons))]
    son1 = Chromosome(son1Polygons, width = self.width, height = self.height)
    son1.calculateAptitude(self.refImgCv2)

    son2 = Chromosome(son2Polygons, width = self.width, height = self.height)
    son2.calculateAptitude(self.refImgCv2)

    return son1,son2

  def roulette(self):
    totalF = sum([i.aptitude for i in self.chromosomes])
    probabilities = [i.aptitude/totalF for i in self.chromosomes]
    mu = len(self.chromosomes)
    selected = np.random.choice(self.chromosomes, mu, p = probabilities)
    return selected

  def sortPopulation(self):
    self.chromosomes.sort(reverse = True, key = lambda x : x.aptitude)

  def getNBest(self, n):
    self.sortPopulation()
    return self.chromosomes[:n]

  def crossPopulation(self, crossPoints,pCross):
    parents = self.roulette()
    sons = []

    for i in range(0,len(parents),2):
      if i == len(parents) - 1:
        sons += [parents[i]]
        continue

      r = np.random.uniform(0,1)
      if r < pCross:
        if np.random.uniform(0,1) < 0.5:
          son1,son2 = self.crossover(parents[i],parents[i+1],crossPoints)
          sons += [son1,son2]
        else:
          son1,son2 = self.crossoverrand(parents[i],parents[i+1])
          sons += [son1,son2]
      else:
        sons += [parents[i],parents[i+1]]
    return sons

  def ageElitism(self, childrenChromosomes):
    childrenChromosomes.sort(reverse = True, key = lambda x : x.aptitude)
    self.sortPopulation()
    if childrenChromosomes[-1].aptitude < self.chromosomes[0].aptitude:
      self.chromosomes = self.chromosomes[:math.ceil(self.mu * 0.15)]+ childrenChromosomes[:(self.mu - math.ceil(self.mu*0.15))]
    else:
      self.chromosomes = childrenChromosomes
  
"""
INITIAL POPULATION
Function to define the initial population
as a set of randomly colored and placed
triangles
"""
def initialPopulation(mu, nPolygons, refImgCv2, width = 100, height = 100):
  colors = [0,0,0,128]
  chromosomes = []
  for i in range(mu):
    polygons = []
    for j in range(nPolygons):
      vertices = [(np.random.randint(0,width),np.random.randint(0,height)) for i in range(3)]
      polygons.append(Polygon(vertices,[np.random.randint(0,255),np.random.randint(0,255),np.random.randint(0,255),40]))
  
    chromosomes.append(Chromosome(polygons,width=width,height=height))
    chromosomes[i].calculateAptitude(refImgCv2)
  return Population(chromosomes,mu,refImgCv2, height, width)

"""
GENETIC ALGORITHM
Joins all previous elements into one function to 
run the genetic algorithm
"""

def AG(n, # N Polygons
       mu, # N sons
       G, # Generations
       refImgCv2, # Image reference
       curGen,
       curPop,
       width = 100, # Image width
       height = 100, # Image height
       crossPoints = 3, # M cross points
       pCross = 0.95, # Cross probability
       pMut = 0.05, # Mutation probability
       vertexP =0.25, # Probability of moving a vertex once in mutation
       vPert = 15, # Perturbation interval of (-a,a) when mutating coordinates
       colorP = 0.25, # Probability of changing an RGBA  value once in mutation
       cPert = 25, # Perturbation interval of (-a,a) when mutating a color
       tPert = 10, # Perturbation interval of (-a,a) when mutating transparency
       spMut = 0.1,# Probability of choosing uniform mutation
       ): 
  
  if curGen == 0:
    population = initialPopulation(mu,n,refImgCv2,width = width,height = height)
    return population
  else:
    sons = copy.deepcopy(Population(curPop.crossPopulation(crossPoints, pCross), mu = mu, refImgCv2 = refImgCv2))
    
    sons.mutate(vertexP = vertexP,
                colorP = colorP,
                vertexPerturbation = vPert,
                colorPerturbation = cPert,
                transparencyPerturbation = tPert,
                secondP = spMut,
                pMut = pMut)
    
    #totalPopulation = Population(sons.chromosomes + population.chromosomes, refImgCv2 = refImgCv2)
    #population = copy.deepcopy(Population(totalPopulation.getNBest(mu), refImgCv2=refImgCv2))
    curPop.ageElitism(sons.chromosomes)
    curPop.sortPopulation()
    sons.sortPopulation()
    image = sons[0].generateImage()
    bio = BytesIO()
    image.save(bio,format = "PNG")
    window["GENIMG"].update(data = bio.getvalue())
    window["text1"].update(f"G = {curGen}")
    window["text2"].update(f"MSE = {sons[0].MSE:.4f}")
    return curPop





"""
MAIN EVENT LOOP
"""

curGen = 0
curPop = None
triangles = int(input("Number of triangles: "))
popSize = int(input("Population size: "))
imgPath = str(input("Image file name: "))
imgHeight = int(input("Image height: "))
imgWidth = int(input("Image width: "))

refImg = Image.open(imgPath).resize((imgWidth,imgHeight))
refImg.save("refImg.png")
refImgCv2 = toCv2(refImg)
curImg = Image.open(imgPath).resize((imgWidth,imgHeight))
curImg.save("curImg.png")

ref_image_col = sg.Column([ [sg.Text('Image Reference')],
                            [sg.Image("refImg.png")],
                            [sg.Text('')],
                            [sg.Text('')]])
gen_image_col = sg.Column([ [sg.Text('Generated Image')],
                            [sg.Image('./curImg.png', key = 'GENIMG')],
                            [sg.Text('G = 0', key = "text1")],
                            [sg.Text('MSE = ---', key = "text2")]])
layout = [[ref_image_col, gen_image_col]]

window = sg.Window('Genetic Image', layout)

while True:
    event, values = window.read(timeout = 50)
    if event == sg.WIN_CLOSED:
        break
    
    curPop = AG(triangles,popSize,5000,refImgCv2,curGen,curPop, width = imgWidth, height = imgHeight)
    curGen += 1
window.close()