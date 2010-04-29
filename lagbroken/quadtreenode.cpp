#include "quadtreenode.h"
#include <iostream>
#include "quadtreestructs.h"
#include <stdlib.h>
#include <limits>
#include "quadtreeexceptions.h"
#include "collisiondetection.h"

using namespace std;

// basic constructor which initilizes the boundary and capacity from paramters and the other meta data to defaults

quadtreenode::quadtreenode(double minX, double minY, double maxX, double maxY, int cap, cacheminder *MCP, string instancedirectory)
{
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   capacity = cap;
   bucket = NULL;
   this->MCP = MCP;
   this->instancedirectory = instancedirectory;
   leaf = true;
   a = b = c = d = NULL;
   numofpoints = 0;
}



// constructor which allows both the boundarys, capacity and the child nodes of the
// quadtree to be defined

quadtreenode::quadtreenode(double minX, double minY, double maxX, double maxY, int cap, quadtreenode* a, quadtreenode* b, quadtreenode* c, quadtreenode* d, cacheminder *MCP, string instancedirectory)
{
   this->minX = minX;
   this->minY = minY;
   this->maxX = maxX;
   this->maxY = maxY;
   capacity = cap;
   bucket = NULL;
   this->MCP = MCP;
   this->instancedirectory = instancedirectory;
   // these checks ensure the new quadtree is legal (that all child nodes
   //  boundarys fall within the parent node).
   // NOTE: WARNING: this checking neglects to check if the child nodes
   // corretly divide the parent node into 4 equal squares

   if (a->minX < minX || a->minY < minY || a->maxX > maxX || a->maxY > maxY)
      throw outofboundsexception("node argument a does not fit within paraent node");

   if (b->minX < minX || b->minY < minY || b->maxX > maxX || b->maxY > maxY)
      throw outofboundsexception("node argument b does not fit within paraent node");

   if (c->minX < minX || c->minY < minY || c->maxX > maxX || c->maxY > maxY)
      throw outofboundsexception("node argument c does not fit within paraent node");

   if (d->minX < minX || d->minY < minY || d->maxX > maxX || d->maxY > maxY)
      throw outofboundsexception("node argument d does not fit within paraent node");





   leaf = false;
   this->a = a;
   this->b = b;
   this->c = c;
   this->d = d;
   numofpoints = 0;
}

// deconstructor for quad tree, this recursivly calls the deconstructor in the
// nodes below it

quadtreenode::~quadtreenode()
{
   delete a;
   delete b;
   delete c;
   delete d;
   if (bucket != NULL)
   {
      delete bucket;
   }
}


// this method prints out a crude representation of the quadtree
// NOTE: this method is purely for debugging purposes and will be very difficult
// to interpret when the tree contains more than 50 or so points

void quadtreenode::print()
{
   if (leaf == true)
   {
      if (numofpoints == 0)
      {
         cout << "(empty)";
      }
      else
      {
         for (int k = 0; k < numofpoints; k++)
         {
            cout << "(" << bucket->getpoint(k).x << " , " << bucket->getpoint(k).y << ")";
         }
      }
      cout << endl << endl;
   }
   else
   {
      a->print();
      b->print();
      c->print();
      d->print();
   }
}

// simple bool check

bool quadtreenode::isLeaf()
{
   return leaf;
}

// this simply checks if the passed point is within the node boundarys

bool quadtreenode::checkbound(point newP)
{
   if (newP.x < minX || newP.x > maxX || newP.y < minY || newP.y > maxY)
   {
      return false;
   }
   return true;
}

// return the boundary of this node in a boundary struct

boundary* quadtreenode::getbound()
{
   boundary *temp = new boundary;
   temp->minX = minX;
   temp->minY = minY;
   temp->maxX = maxX;
   temp->maxY = maxY;
   return temp;
}

// this method takes a point and returns the child node that the point falls
// into, if none an exception is thrown

quadtreenode* quadtreenode::pickchild(point newP)
{
   if (leaf)
   {
      return NULL;
   }
   if (a->checkbound(newP))
   {
      return a;
   }
   else if (b->checkbound(newP))
   {
      return b;
   }
   else if (c->checkbound(newP))
   {
      return c;
   }
   else if (d->checkbound(newP))
   {
      return d;
   }
   else
   {
      throw outofboundsexception("failed to fit into any of the four child nodes, big problem");
   }
}





// this method inserts a point into the node. 
// NOTE: this method does not contain functionality for finding the correct node to insert into
// save for when the correct node overflows and 4 children need to be created and reporting
// wrong nodes.

bool quadtreenode::insert(point newP)
{

   // if the point dosen't belong in this subset of the tree return false
   if (newP.x < minX || newP.x > maxX || newP.y < minY || newP.y > maxY)
   {
      return false;
   }
   else
   {
      // if the node has overflowed and is a leaf
      if ((numofpoints + 1) > capacity && leaf == true)
      {
         // this bucket is full, create four new buckets
         // and populate them.
         // NOTE: because it is possible to create a node with one, any or all of
         // the child nodes already created (see constructors ^) new child nodes
         // are only created where needed.
         // WARNING: because there is nothing to control the boundary of childs
         // created during construction this may lead to overlapping children
         if (a == NULL)
            a = new quadtreenode(minX, minY + ((maxY - minY) / 2.0), minX + ((maxX - minX) / 2.0), maxY, capacity, MCP, instancedirectory);
         if (b == NULL)
            b = new quadtreenode(minX + ((maxX - minX) / 2.0), minY + ((maxY - minY) / 2.0), maxX, maxY, capacity, MCP, instancedirectory);
         if (c == NULL)
            c = new quadtreenode(minX, minY, minX + ((maxX - minX) / 2.0), minY + ((maxY - minY) / 2.0), capacity, MCP, instancedirectory);
         if (d == NULL)
            d = new quadtreenode(minX + ((maxX - minX) / 2.0), minY, maxX, minY + ((maxY - minY) / 2.0), capacity, MCP, instancedirectory);

         for (int k = 0; k < numofpoints; k++)
         {
            point bob = bucket->getpoint(k);
            // attept to insert each point in turn into the child nodes
            if (a->insert(bob))
            {
               //  cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << a->minX << " " << a->maxX << ")" << endl;
            }
            else if (b->insert(bob))
            {
               // cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << b->minX << " " << b->maxX << ")" << endl;
            }
            else if (c->insert(bob))
            {
               //cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << c->minX << " " << c->maxX << ")" << endl;
            }
            else if (d->insert(bob))
            {
               // cout << bob.x << "/" << bob.y << " old inserted into bucket " << bucket << "(" << d->minX << " " << d->maxX << ")" << endl;
            }
            else
            {
               throw outofboundsexception("failed to insert old point into any of the four child nodes, big problem");
            }

         }


         // clean up node and turn into non leaf
         delete bucket;
         bucket = NULL;
         leaf = false;

         // insert the new point that caused the overflow
         if (a->insert(newP))
         {
            return true;
         }
         if (b->insert(newP))
         {
            return true;
         }
         if (c->insert(newP))
         {
            return true;
         }
         if (d->insert(newP))
         {
            return true;
         }
         throw outofboundsexception("failed to insert new point into any of the four child nodes, big problem");
      }

      // if the node falls within the boundary but this node not a leaf
      if (leaf == false)
      {
         return false;
      }
         // if the node falls within the boundary and will not cause an overflow
      else
      {
         // insert new point
         if (bucket == NULL)
         {
            bucket = new pointbucket(capacity, minX, minY, maxX, maxY, MCP, instancedirectory);
            //cout << "bucket " << bucket << " created " << endl;
         }
         
         bucket->setpoint(newP);
         
         numofpoints++;
         return true;
      }
   }
}


// recursivly sort each node

void quadtreenode::sort(int ( * comparator) (const void *, const void *))
{
   if (!leaf)
   {
      a->sort(comparator);
      b->sort(comparator);
      c->sort(comparator);
      d->sort(comparator);
   }

   if (bucket == NULL)
   {
      return;
   }
   // the qsort function takes a function to compare elements, by passing
   // different functions the attribute by which the points are sorted is controlled
   //    qsort(bucket->points, bucket->numberofpoints, sizeof (point), comparator);
}

// if this is a leaf and the bucket is NULL then it is empty,
// sort of, WARNING: this dosen't take account of the damn constructor setting
// childs with no thought for the consiquences

bool quadtreenode::isEmpty()
{
   if (leaf && bucket == NULL)
   {
      return true;
   }
   return false;
}




//  checks for line line intersection between lines (x1,y1 -> x2,y2) , (x3,y3 -> x4,y4)

bool lineintersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{

   // if you can rule out lines because both points are to the left, right, top or bottom
   // do so
   if (x1 > x3 && x2 > x3 && x1 > x4 && x2 > x4)
   {
      return false;
   }
   if (x1 < x3 && x2 < x3 && x1 < x4 && x2 < x4)
   {
      return false;
   }
   if (y1 > y3 && y2 > y3 && y1 > y4 && y2 > y4)
   {
      return false;
   }
   if (y1 < y3 && y2 < y3 && y1 < y4 && y2 < y4)
   {
      return false;
   }






   // find the x,y value of the intersect
   double x = (((x1 * y2)-(y1 * x2))*(x3 - x4) - (x1 - x2)*((x3 * y4)-(y3 * x4))) / (((x1 - x2)*(y3 - y4)) - ((y1 - y2)*(x3 - x4)));
   double y = (((x1 * y2)-(y1 * x2))*(y3 - y4) - (y1 - y2)*((x3 * y4)-(y3 * x4))) / (((x1 - x2)*(y3 - y4)) - ((y1 - y2)*(x3 - x4)));

   // check if both are segments of the same line
   if (x != x)
   {
      // check if they overlap
      if (y1 > y3 && y2 > y3 && y1 > y4 && y2 > y4)
      {
         return false;
      }
      if (y1 < y3 && y2 < y3 && y1 < y4 && y2 < y4)
      {
         return false;
      }
      return true;
   }
   // check if the lines are parallel
   if (std::numeric_limits<double>::infinity() == x)
   {
      return false;
   }



   bool flag = false;
   double err = 0.000001;

   //check if the x value falls within the line segments
   // if the first line is vertical you have to check the y value
   if (x1 == x2)
   {
      if ((y1 - y2 + err) > 0 && (y - y1 - err) < 0 && (y - y2 + err) > 0)
      {
         flag = true;
      }
      else if ((y1 - y2 - err) < 0 && (y - y1 + err) > 0 && (y - y2 - err) < 0)
      {
         flag = true;
      }
   } // if the first line is not vertical just check if the x intersection falls
      // within both line segments
   else if ((x1 - x2 + err) > 0 && (x - x1 - err) < 0 && (x - x2 + err) > 0)
   {
      flag = true;
   }
   else if ((x1 - x2 - err) < 0 && (x - x1 + err) > 0 && (x - x2 - err) < 0)
   {
      flag = true;
   }
   else
   {
      return false;
   }


   // if the second line is vertical you have to check the y value
   if (x3 == x4)
   {
      if ((y3 - y4 + err) > 0 && (y - y3 - err) < 0 && (y - y4 + err) > 0)
      {
         if (flag)
         {
            return true;
         }
      }
      else if ((y3 - y4 - err) < 0 && (y - y3 + err) > 0 && (y - y4 - err) < 0)
      {
         if (flag)
         {
            return true;
         }
      }
   } // if the second line is not vertical just check if the x intersection falls
      // within both line segments
   else if ((x3 - x4 + err) > 0 && (x - x3 - err) < 0 && (x - x4 + err) > 0)
   {
      if (flag)
      {
         return true;
      }
   }
   else if ((x3 - x4 - err) < 0 && (x - x3 + err) > 0 && (x - x4 - err) < 0)
   {
      if (flag)
      {
         return true;
      }
   }


   return false;
}

// this method takes the forumula for 4 lines and checks if the x and y arguments are within them
bool OBBpoint(double m1, double c1, double m2, double c2, double m3, double c3, double m4, double c4, double x, double y)
{
   // find the point where an imaginary vertical line through the x value intersects with each line
   double intersec1 = m1 * x + c1;
   double intersec2 = m2 * x + c2;
   double intersec3 = m3 * x + c3;
   double intersec4 = m4 * x + c4;

   // if the xy point is between the intersections with the oposite sides of the rectangle the
   // point is within the rectangle (think about it, it works)
   double err = 0.000001;
   if (((intersec1 - y + err) > 0 && (intersec3 - y - err) < 0) || ((intersec1 - y - err) < 0 && (intersec3 - y + err) > 0))
   {
      if (((intersec2 - y + err) > 0 && (intersec4 - y - err) < 0) || ((intersec2 - y - err) < 0 && (intersec4 - y + err) > 0))
      {
         return true;
      }
   }
   return false;
}

// method to add the bucket or recure into the child nodes
// NOTE : this has been seperated from the advsubset method to allow a more logical format to that method
/*
void quadtreenode::addsubset(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, vector<pointbucket*> *buckets)
{
   if (!leaf)
   {
      // call subset recursivly on child nodes
      a->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      b->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      c->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
      d->advsubset(x1, y1, x2, y2, x3, y3, x4, y4, buckets);
   }
   else
   {
      if (bucket != NULL)
      {
         // add bucket to the vector of buckets
         buckets->push_back(bucket);
      }

   }

}*/



// this method takes 4 points that describe a rectangle of any orientation and 
// fills the passed vector with buckets that collide with this rectangle

void quadtreenode::advsubset(double *Xs, double *Ys, int size, vector<pointbucket*> *buckets)
{

   // calculate the equations for the lines

   //  NOTE : there is no check for axis orientated lines, this is handled by the quadtree

   // work out the forumla for each of the four lines described by the four points (y=mx+c)

   

  if(AOrec_NAOrec(minX, minY, maxX, maxY, Xs, Ys, size))
   {
      if(!leaf)
      {
         a->advsubset(Xs, Ys, size, buckets);
         b->advsubset(Xs, Ys, size, buckets);
         c->advsubset(Xs, Ys, size, buckets);
         d->advsubset(Xs, Ys, size, buckets);
      }
      else
      {
         if(bucket != NULL)
         {
            buckets->push_back(bucket);
         }
      }
   }

   

}