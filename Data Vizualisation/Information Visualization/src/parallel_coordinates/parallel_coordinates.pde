final int CANVAS_WIDTH = 900;
final int CANVAS_HEIGHT = 600;

ParallelCoordinates pc;

void setup(){
  size(800,600);
  background(255);
  pc = new ParallelCoordinates("bupa.data");
}

void draw(){
  //Axis a = new Axis("try",2,4.5,100,100,100,500,5);
  //a.drawAxis();
  background(255);
  pc.drawGraph();
}

void mouseClicked(){
  pc.mouseClicked();
}