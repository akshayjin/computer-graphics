#ifndef TRACKBALL_H
#define TRACKBALL_H


class Trackball
{
   private :
         // Quaternion information
      float m_lastquat[4] ;
      float m_currquat[4] ;
   public :
         // Constructors
      Trackball () ;
      Trackball ( const Trackball& ) ;
         // Assignment
      Trackball& operator= ( const Trackball& ) ;
         // Rotate virtual trackball given 2d start point
         //    and end points expressed between ([-1,1],[-1,1])
      void rotate ( float p1x, float p1y, float p2x, float p2y ) ;
         // Rotate virtual trackball given an angle in degrees
         //    and an axis of rotation
      void rotateAngle ( float phi, float ax, float ay, float az ) ;
         // Apply last rotation again, scaled by "friction"
      void spin ( float friction = 1.0 ) ;
         // Reset all rotations to zero
      void reset () ;
         // Returns a rotation matrix in m
      void rotationMatrix ( float m[4][4] ) ;
} ; // class Trackball


#endif // TRACKBALL_H
