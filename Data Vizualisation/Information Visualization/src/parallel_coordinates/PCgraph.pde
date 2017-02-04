class ParallelCoordinates{
  
  public int dimension;
  public DataHandler dh;
  public ArrayList<Axis>axes;
  boolean mouseClicked = false;
  int axisSelected = -1;
  
  public ParallelCoordinates(String filename){
    dh = new DataHandler(filename);
    dh.tableInfo();
    dimension = dh.getDimension();
    ArrayList<String> attributes = dh.getColumnNames();
    axes = new ArrayList<Axis>();
    for(int i=0;i<attributes.size()-1;i++){
      String attribute = attributes.get(i);
      Axis a = new Axis(attribute,(float)Math.ceil(dh.getMax(attribute)),(float)Math.floor(dh.getMin(attribute)),100 * (i+1),100,100 * (i+1),500,5);
      axes.add(a);
    }
  }
  
  void mouseClicked(){
    println("mouse clicked, axis selected = " + axisSelected);
    if (axisSelected != -1){
     axisSelected = -1;
     return;
    }
    for(int i=0;i<axes.size();i++){
       if(mouseY > axes.get(i).startY -35 && mouseY < axes.get(i).startY - 25 && mouseX > axes.get(i).startX-15 && mouseX < axes.get(i).endX+15){
         axisSelected = i;
         break;
       }
     }
  }
  
  public void drawGraph(){
    
    //Check if axis is being moved
    int i=0;
    if(mousePressed){
     if(axisSelected >= 0 ){
         axes.get(axisSelected).startX = mouseX;
         axes.get(axisSelected).endX = mouseX;
         if(axisSelected < 5 && mouseX > (axes.get(axisSelected+1).startX) - 1 && mouseX < (axes.get(axisSelected+1).startX) + 1){
           Axis a1 = axes.get(axisSelected);
           Axis a2 = axes.get(axisSelected+1);
           a2.startX -= 100;
           a2.endX -= 100;
           axes.set(axisSelected,a2);
           axes.set(axisSelected+1,a1);
           axisSelected +=1;
         }
         else if(axisSelected >0 && mouseX > (axes.get(axisSelected-1).startX) - 1 && mouseX < (axes.get(axisSelected-1).startX) + 1){
          Axis a1 = axes.get(axisSelected);
          Axis a2 = axes.get(axisSelected-1);
          a2.startX += 100;
          a2.endX += 100;
          axes.set(axisSelected,a2);
          axes.set(axisSelected-1,a1);
          axisSelected -=1;
         }
     }

    }
    
    
    
    //Draw each row
    int rowCount = 0;
    for (TableRow row : dh.table.rows()) {
        rowCount++;
        for(i=0;i<axes.size()-1;i++){
          float val1 = row.getFloat(axes.get(i).title);
          float p1[] = axes.get(i).getCoordinates(val1);
          float val2 = row.getFloat(axes.get(i+1).title);
          float p2[] = axes.get(i+1).getCoordinates(val2);
          if(row.getInt("selector")==1)
            stroke(255,0,0);
          else
            stroke(0,255,0);
          line(p1[0],p1[1],p2[0],p2[1]);
        }
        if(rowCount==10)
          break;
    }
    
    // Draw axes
    for(Axis a:axes){
      a.drawAxis();
    }
  }
}