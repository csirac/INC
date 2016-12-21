#!/bin/bash
# A script that generates graph images

TYPE="BA"
NODES=30
PROB_OR_EDGES=2
IMAGE_FILE="DummyGraphs/dummy6.png"
GRAPH_INPUT="DummyGraphs/dummy6.el"
GRAPH_ML="dummy6.graphml"

#./main $TYPE$NODES:$PROB_OR_EDGES mygraph.graphml
./main $GRAPH_INPUT $GRAPH_ML
python DisplayGraph.py $GRAPH_ML $IMAGE_FILE
display $IMAGE_FILE

