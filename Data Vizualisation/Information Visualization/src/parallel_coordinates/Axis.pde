class Axis{
  public float range_min;
  public float range_max;
  public int divisions;
  public String title;
 
  private float startX;
  private float startY;
  private float endX;
  private float endY;
  
  public Axis(String title, float max, float min, float startX, float startY, float endX, float endY, int div){
    this.title = title;
    this.range_min = min;
    this.range_max = max;
    this.divisions = div;
    this.startX = startX;
    this.endX = endX;
    this.startY = startY;
    this.endY = endY;
  }
  
  public void drawAxis(){
    int k1 = 0;
    int k2 = divisions;
    strokeWeight(1.5);
    stroke(0);
    for(int i=0;i<=divisions;i++){
      float xp = (k1*endX+k2*startX)/(k1+k2);
      float yp = (k1*endY+k2*startY)/(k1+k2);
      line(xp,yp,xp-5,yp);
      Float value = range_min + i*(range_max-range_min)/divisions;
      fill(0);
      text(value.toString(),xp-30,yp);
      k1++;
      k2--;
    }
    line(startX,startY,endX,endY);
    text(title,startX-10,startY-30);
  }
  
  public float[] getCoordinates(float value){
    float[] coordinates = {0,0};
    float k1 = value - range_min;
    float k2 = range_max - value;
    coordinates[0] = (k1*endX+k2*startX)/(k1+k2);
    coordinates[1] = (k1*endY+k2*startY)/(k1+k2);
    return coordinates;
  }
  
}