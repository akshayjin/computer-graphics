TMGraph graph;
float sval = .25;

void setup(){
  size(700,700);
  background(255);
  graph = new TMGraph("car.csv");
  graph.drawGraph();
}

void draw(){
  if(mousePressed) { 
    if(mouseX < 350)
      sval += 0.005;
    else
      sval -= 0.005;
    scale(sval);
    background(255);
    graph.drawGraph();
  } 
  sval = constrain(sval, .1, 2.0);
}