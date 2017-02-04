class Rectangle{
 public float startX;
 public float startY;
 public float endX;
 public float endY;
 public String title;
 public ArrayList<Rectangle> children;
 public int numChildren;
 public boolean horizontal;
 public int level;
 
 public Rectangle(float startX,float startY,float endX,float endY,String title,boolean horizontal,int level,DataHandler dh){
   this.startX = startX;
   this.startY = startY;
   this.endX = endX;
   this.endY = endY;
   this.title = title;
   this.horizontal = horizontal;
   this.level = level;
   children = new ArrayList<Rectangle>();
   setChildren(dh);
 }
 
 public void setChildren(DataHandler dh){
   ArrayList<String> attributes = dh.getColumnNames();
   if(level == attributes.size()-1)
     return;
   HashMap<String,Integer> hm = dh.attributeValues(attributes.get(level),title,level);  
   int total = 0;
   for(String key: hm.keySet()){
     total += hm.get(key);
   }
   float epsilon = 4;
   if(horizontal){
     float beginX = startX + epsilon;
     float beginY = startY + epsilon;
     float lastX = endX - epsilon;
     float lastY = endY - epsilon;
     float unit = (lastX-beginX)/total;
     for(String key: hm.keySet()){
       Rectangle temp = new Rectangle(beginX,beginY,beginX+hm.get(key)*unit,lastY,title+"."+key,false,level+1,dh);
       beginX = beginX+hm.get(key)*unit;
       children.add(temp);
     }
   }
   else{
     float beginX = startX + epsilon;
     float beginY = startY + epsilon;
     float lastX = endX - epsilon;
     float lastY = endY - epsilon;
     float unit = (lastY-beginY)/total;
     for(String key: hm.keySet()){
       Rectangle temp = new Rectangle(beginX,beginY,lastX,beginY+hm.get(key)*unit,title+"."+key,true,level+1,dh);
       beginY = beginY+hm.get(key)*unit;
       children.add(temp);
     }
   }
 }
   
 public void drawRectangle(DataHandler dh){
   //println(startX+" "+startY+" "+endX+" "+endY);
   if(level==6){
     String[] temp = splitTokens(title,".");
     String[] attributes = new String[temp.length-1];
     for(int i=1;i<temp.length;i++)
       attributes[i-1] = temp[i];
     String cls = dh.findClass(attributes);
     if(cls.compareTo("unacc")==0 || cls.compareTo("1")==0)
       fill(255,0,0,153);
     else if(cls.compareTo("acc")==0 || cls.compareTo("2")==0)
       fill(153,0,153,153);
     else if(cls.compareTo("good")==0)
       fill(0,153,153,153);
     else
       fill(0,255,0,153);
     stroke(255);
     rect(startX,startY,endX-startX,endY-startY); 
     fill(255);
     text(cls.charAt(0),(endX+startX)/2,(endY+startY)/2);
   }
   else{
     fill(level*30);
     stroke(255);
     rect(startX,startY,endX-startX,endY-startY); 
     for(Rectangle r: children)
       r.drawRectangle(dh);
   }

 }
}