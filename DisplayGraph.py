import networkx as nx
import sys
import matplotlib.pyplot as plt
from PIL import Image, ImageDraw, ImageFont


# Get the text to print for each attribute
# graph is a networkx graph
# attributes is a list or tuple
def getAttributeText(graph, attributes):

   text = []

   for attribute in attributes:
      if attribute in graph.graph:
         text.append(attribute + ": " + str(graph.graph[attribute]))

   return text
#END


# Take in a networkx graph and return an agraph graph, that can be printed
# as a graphviz. Also, change a few things to make the graphviz look good.
def makeAGraph(graph):

   # Make agraph graph
   agraph = nx.to_agraph(graph)

   #Help it look pretty
   agraph.graph_attr.update(splines="true")
   agraph.node_attr["shape"] = "circle"
   agraph.node_attr["style"] = "filled"
   agraph.node_attr["fillcolor"] = "#68b7a9"
   agraph.node_attr["color"] = "#41726a"
   agraph.edge_attr["color"] = "#4b5956"
   agraph.edge_attr["penwidth"] = "0.5"
   agraph.node_attr["width"] = 0.1
   agraph.node_attr["fizedsize"] = "true"

   for node in agraph.nodes_iter():
      node.attr["label"] = ""

   return agraph
#END


# Write each element of text as a row in the upper right corner of the image
# Image name is image_filename
# List of each row text in text
def writeText(image_filename, text):

   # open the image we will write on
   image = Image.open(image_filename)

   # get the font we'll use
   font = ImageFont.truetype("/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-B.ttf", 16)

   # something we need for this library ...
   draw = ImageDraw.Draw(image)

   # write each elt on its own row on the top right of the image
   # space to left is 10
   # first row is space from top 10, space between is 20
   row = 10
   for i in text:
      draw.text((10, row), i, (0, 0, 0), font=font)
      row = row + 20

   image.save(image_filename)
#END


# Read in graph from graphml file, build networkx graph, display graphviz image
# arg1 is the graphml file "graph.graphml"
# arg2 is the output image file name "graph.png"
if __name__ == "__main__":

   # graphml file name
   graph_file = sys.argv[1]

   # where we want to save our image
   output_file = sys.argv[2]

   #load as networkx graph
   graph = nx.read_graphml(graph_file)

   # make an agraph graph to print graphviz
   agraph = makeAGraph(graph)
   # get the right layout and draw
   agraph.layout("fdp")
   agraph.draw(output_file)

   # attributes of the graph we will want printed on the image
   attributes = ("type", "GCC", "ALCC", "ECC0", "ECC1","ECC2","ECC3","ECC4", "ECC5")
   # the text we wil print, with info about the graph
   attribute_text = getAttributeText(graph, attributes)

   # need to re-open image and write text
   writeText(output_file, attribute_text)

#END


