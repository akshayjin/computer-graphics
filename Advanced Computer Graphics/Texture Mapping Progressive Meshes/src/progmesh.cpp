#include <stdio.h>  
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream> 
#include <algorithm> 
#include <set>
#include <queue>
#include <iterator>
#include <vector>

#include "vector.h"
#include "list.h"
#include "progmesh.h"

// bunny 0.6; 0.1
// teapot 25
// eagle 4
float STOPPING_THRESHOLD = 0.035;
#define LOADING_SPEED 200

int NUM_CHARTS = 0;

class Triangle;
class Vertex;

class Triangle {
  public:
	Vertex *         vertex[3]; // the 3 points that make this tri
	int chart_id;
	Vector           normal;    // unit vector othogonal to this face
	                 Triangle(Vertex *v0,Vertex *v1,Vertex *v2);
	                 ~Triangle();
	void             ComputeNormal();
	void             ReplaceVertex(Vertex *vold,Vertex *vnew);
	int              HasVertex(Vertex *v);
};
class Vertex {
  public:
	Vector           position; // location of point in euclidean space
	int              id;       // place of vertex in original list
	List<Vertex *>   neighbor; // adjacent vertices
	List<Triangle *> face;     // adjacent triangles
	bool			 onBoundary;
	float            objdist;  // cached cost of collapsing edge
	Vertex *         collapse; // candidate vertex for collapse
	                 Vertex(Vector v,int _id);
	                 ~Vertex();
	void             RemoveIfNonNeighbor(Vertex *n);
};
List<Vertex *>   vertices;
List<Triangle *> triangles;

Triangle::Triangle(Vertex *v0,Vertex *v1,Vertex *v2){
	assert(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;
	ComputeNormal();
	triangles.Add(this);
	for(int i=0;i<3;i++) {
		vertex[i]->face.Add(this);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
}

Triangle::~Triangle(){
	int i;
	triangles.Remove(this);
	for(i=0;i<3;i++) {
		if(vertex[i]) vertex[i]->face.Remove(this);
	}
	for(i=0;i<3;i++) {
		int i2 = (i+1)%3;
		if(!vertex[i] || !vertex[i2]) continue;
		vertex[i ]->RemoveIfNonNeighbor(vertex[i2]);
		vertex[i2]->RemoveIfNonNeighbor(vertex[i ]);
	}
}
int Triangle::HasVertex(Vertex *v) {
	return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
}
void Triangle::ComputeNormal(){
	Vector v0=vertex[0]->position;
	Vector v1=vertex[1]->position;
	Vector v2=vertex[2]->position;
	normal = (v1-v0)*(v2-v1);
	if(magnitude(normal)==0)return;
	normal = normalize(normal);
}
void Triangle::ReplaceVertex(Vertex *vold,Vertex *vnew) {
	assert(vold && vnew);
	assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
	if(vold==vertex[0]){
		vertex[0]=vnew;
	}
	else if(vold==vertex[1]){
		vertex[1]=vnew;
	}
	else {
		assert(vold==vertex[2]);
		vertex[2]=vnew;
	}
	int i;
	vold->face.Remove(this);
	assert(!vnew->face.Contains(this));
	vnew->face.Add(this);
	for(i=0;i<3;i++) {
		vold->RemoveIfNonNeighbor(vertex[i]);
		vertex[i]->RemoveIfNonNeighbor(vold);
	}
	for(i=0;i<3;i++) {
		assert(vertex[i]->face.Contains(this)==1);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
	ComputeNormal();
}

Vertex::Vertex(Vector v,int _id) {
	position =v;
	id=_id;
	onBoundary = false;
	vertices.Add(this);
}

Vertex::~Vertex(){
	//~ assert(face.num==0);
	while(neighbor.num) {
		neighbor[0]->neighbor.Remove(this);
		neighbor.Remove(neighbor[0]);
	}
	vertices.Remove(this);
}
void Vertex::RemoveIfNonNeighbor(Vertex *n) {
	// removes n from neighbor list if n isn't a neighbor.
	if(!neighbor.Contains(n)) return;
	for(int i=0;i<face.num;i++) {
		if(face[i]->HasVertex(n)) return;
	}
	neighbor.Remove(n);
}


/*******************Chart Partition*************************/
class Chart {
	public:
		int id;
		std::vector<Triangle *> chart_triangles;
		std::set<Vertex *> chart_vertices;
		std::set<Vertex *> chart_boundary;
		std::set<int> neighbors;
		Chart(int i,Triangle * t);
		Chart(Chart* c);
		~Chart();
};

Chart::Chart(int i,Triangle * t){
	id = i;
	chart_triangles.push_back(t);
	chart_vertices.insert(t->vertex[0]);
	chart_vertices.insert(t->vertex[1]);
	chart_vertices.insert(t->vertex[2]);
	chart_boundary.insert(t->vertex[0]);
	chart_boundary.insert(t->vertex[1]);
	chart_boundary.insert(t->vertex[2]);
	t->chart_id = id;
}

Chart::Chart(Chart* c){
	id = c->id;
	chart_triangles.insert(chart_triangles.end(), c->chart_triangles.begin(), c->chart_triangles.end());
	chart_vertices.insert(c->chart_vertices.begin(), c->chart_vertices.end());
	chart_boundary.insert(c->chart_boundary.begin(), c->chart_boundary.end());
	neighbors.insert(c->neighbors.begin(), c->neighbors.end());
}

Chart::~Chart() {
	chart_triangles.clear();
	chart_vertices.clear();
	chart_boundary.clear();
	neighbors.clear();
}

template<class Set1, class Set2> 
bool is_disjoint(const Set1 &set1, const Set2 &set2)
{
    if(set1.empty() || set2.empty()) return true;

    typename Set1::const_iterator 
        it1 = set1.begin(), 
        it1End = set1.end();
    typename Set2::const_iterator 
        it2 = set2.begin(), 
        it2End = set2.end();

    if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;

	int count = 0;
    while(it1 != it1End && it2 != it2End)
    {
        if(*it1 == *it2) count++;
        if(count>1)
			return false;
        if(*it1 < *it2) { it1++; }
        else { it2++; }
    }

    return true;
}

std::vector<Chart*> charts;

int defineFinalBoundary() {
	int total = 0;
	for(int i=0;i<vertices.num;i++) {
		int count = 0;
		for(int j=0;j<charts.size();j++) {
			if(charts[j]){
				const bool is_in = charts[j]->chart_vertices.find(vertices[i]) != charts[j]->chart_vertices.end();
				if(is_in)
					count++;
			}
		}
		if(count>=2){
			vertices[i]->onBoundary = true;
			total++;
		}
	}
	return total;
}

int countNumCharts() {
	int count = 0;
	for(int i=0;i<charts.size();i++)
		if(charts[i])
			count++;
	return count;
}

bool checkIfAdjacent(Chart* c1,Chart* c2) {
	return !is_disjoint(c1->chart_vertices,c2->chart_vertices);
}

bool fillNeighbours() {
	for(int i=0;i<charts.size();i++) {
		for(int j=i+1;j<charts.size();j++) {
			if(checkIfAdjacent(charts[i],charts[j])){
				charts[i]->neighbors.insert(charts[j]->id);
				charts[j]->neighbors.insert(charts[i]->id);
			}
		}
	}
}

typedef struct MergeOp{
	int c1;
	int c2;
	float cost;
}MergeOp;

struct LessThanByMergeCost
{
  bool operator()(const MergeOp& lhs, const MergeOp& rhs) const
  {
    return lhs.cost > rhs.cost;
  }
};

float calculatePlanarity(std::set<Vertex *> chart_vertices) {
	int n = chart_vertices.size();
	float centroid[3] = {0.0,0.0,0.0};
	for ( auto it = chart_vertices.begin(); it != chart_vertices.end(); it++ ) {
		centroid[0] += ((*it)->position).x;
		centroid[1] += ((*it)->position).y;
		centroid[2] += ((*it)->position).z;
	}
	centroid[0] /= n;
	centroid[1] /= n;
	centroid[2] /= n;
	
	float xx = 0, xy = 0, xz = 0, yy = 0, yz = 0, zz = 0; 
	
	for ( auto it = chart_vertices.begin(); it != chart_vertices.end(); it++ ) {
		float r[3] = {((*it)->position).x - centroid[0], ((*it)->position).y - centroid[1], ((*it)->position).z - centroid[2]};
		xx += r[0] * r[0];
        xy += r[0] * r[1];
        xz += r[0] * r[2];
        yy += r[1] * r[1];
        yz += r[1] * r[2];
        zz += r[2] * r[2];
	}
	
	float det_x = yy*zz - yz*yz;
    float det_y = xx*zz - xz*xz;
    float det_z = xx*yy - xy*xy;
	return std::max(det_x,std::max(det_y,det_z));
}

float calculateCompactness(std::set<Vertex *> chart_boundary) {
	float squared_perimeter = 0.0;
	
	int i=0;
	for ( auto it = chart_boundary.begin(); it != chart_boundary.end(); it++ ) {
		auto next = std::next(it,1);
		if(next == chart_boundary.end())
			break;
		squared_perimeter += ((((*it)->position).x-((*next)->position).x)*(((*it)->position).x-((*next)->position).x)
		 + (((*it)->position).y-((*next)->position).y)*(((*it)->position).y-((*next)->position).y)
		 + (((*it)->position).z-((*next)->position).z)*(((*it)->position).z-((*next)->position).z));
	}
	
	return squared_perimeter;
}

void MergeTwoCharts(Chart* c1, Chart* c2) {
	// Merge c2 into c1
	// All vertices of c2 into c1
	c1->chart_triangles.insert(std::end(c1->chart_triangles), std::begin(c2->chart_triangles), std::end(c2->chart_triangles));
	c1->chart_vertices.insert(c2->chart_vertices.begin(), c2->chart_vertices.end());
	c1->neighbors.insert(c2->neighbors.begin(), c2->neighbors.end());
	for ( auto it = c2->neighbors.begin(); it != c2->neighbors.end(); it++ ) {
		if(charts[*it])
			charts[*it]->neighbors.insert(c1->id);
	}
	
	
	// Update triangles chart ids
	for(int i=0;i<c2->chart_triangles.size();i++)
		c2->chart_triangles[i]->chart_id = c1->id;
	// Boundary of c1 and c2 - Common boundary
	std::set<Vertex *> new_boundary;
	set_symmetric_difference(c1->chart_boundary.begin(), c1->chart_boundary.end(), c2->chart_boundary.begin(), c2->chart_boundary.end(), inserter(new_boundary, new_boundary.end()));
	//~ std::cout << "Boundary Size before = " << c1->chart_boundary.size() << std::endl;
	c1->chart_boundary = new_boundary;
	int id2 = c2->id;
	delete c2;
	charts[id2] = NULL;
}

MergeOp costOfMerge(Chart* c1, Chart* c2){
	// Calculate using garfields paper
	
	float cost = calculatePlanarity(c1->chart_vertices) + 3*calculateCompactness(c1->chart_boundary) + calculatePlanarity(c2->chart_vertices) + 3*calculateCompactness(c2->chart_boundary);
	MergeOp mop = {c1->id,c2->id,cost};
	return mop;
}

int MergeCharts(){
	fillNeighbours();
	std::cout << "Neighbours Filled" << std::endl;
	// set threshold
	std::priority_queue<MergeOp, std::vector<MergeOp>, LessThanByMergeCost> pq;
	for(int i=0;i<charts.size();i++) {
		//~ std::cout << "Processing Chart " << i << std::endl;
		if(charts[i]) {
			for ( auto it = charts[i]->neighbors.begin(); it != charts[i]->neighbors.end(); it++ ) {
				if(charts[*it])
					pq.push(costOfMerge(charts[i],charts[*it]));
			}
		}
	}
	std::cout << "Priority Queue Filled" << std::endl;
	int numCharts = countNumCharts();
	while(1){
		
		MergeOp min_cost_merge = pq.top();
		pq.pop();
		while(charts[min_cost_merge.c1] == NULL || charts[min_cost_merge.c2] == NULL){
			min_cost_merge = pq.top();
			pq.pop();
		}
		if(numCharts % LOADING_SPEED == 0)
			std::cout << "Number of charts = " << numCharts << " Cost reached = " <<  min_cost_merge.cost << "/" << STOPPING_THRESHOLD << "PQ size = " << pq.size() << std::endl;
		
		if(min_cost_merge.cost > STOPPING_THRESHOLD || numCharts<10 || pq.size()==0)
			break;
		//merge c1 and c2
		//~ std::cout << "Merging Charts " << min_cost_merge.c1 << " and " << min_cost_merge.c2 << "cost: " << min_cost_merge.cost << std::endl;
		MergeTwoCharts(charts[min_cost_merge.c1],charts[min_cost_merge.c2]);
		numCharts--;
		int i = min_cost_merge.c1;
		if(charts[i]) {
			for ( auto it = charts[i]->neighbors.begin(); it != charts[i]->neighbors.end(); it++ ) {
				if(charts[*it] && charts[*it]->id != i)
					pq.push(costOfMerge(charts[i],charts[*it]));
			}
		}
	}
	int boundary_len = defineFinalBoundary();
	std::cout << "Length of chart boundary = " << boundary_len << std::endl;
	NUM_CHARTS = countNumCharts();
	std::cout << "Final Number of charts = " << NUM_CHARTS << std::endl;
	return boundary_len;
}
/***********************************************************/

// Calculated using Stan Melax formulation in GameDev Magzine 1998

float ComputeEdgeCollapseCost(Vertex *u,Vertex *v) {
	// deviation if we collapse edge uv by moving u to v
	int i;
	float edgelength = magnitude(v->position - u->position);
	float curvature=0;

	List<Triangle *> sides;
	for(i=0;i<u->face.num;i++) {
		if(u->face[i]->HasVertex(v)){
			sides.Add(u->face[i]);
		}
	}
	for(i=0;i<u->face.num;i++) {
		float mincurv=1; 
		for(int j=0;j<sides.num;j++) {
			float dotprod = u->face[i]->normal ^ sides[j]->normal;
			mincurv = std::min(mincurv,(1-dotprod)/2.0f);
		}
		curvature = std::max(curvature,mincurv);
	}
	return edgelength * curvature;
}

void ComputeEdgeCostAtVertex(Vertex *v) {
	if(v->neighbor.num==0) {
		v->collapse=NULL;
		v->objdist=-0.01f;
		return;
	}
	v->objdist = 1000000;
	v->collapse= NULL;
	for(int i=0;i<v->neighbor.num;i++) {
		float dist = ComputeEdgeCollapseCost(v,v->neighbor[i]);
		if(dist<v->objdist) {
			v->collapse=v->neighbor[i];  
			v->objdist=dist;
		}
	}
}
void ComputeAllEdgeCollapseCosts() {
	for(int i=0;i<vertices.num;i++) {
		ComputeEdgeCostAtVertex(vertices[i]);
	}
}

void Collapse(Vertex *u,Vertex *v){
	if(!v) {
		delete u;
		return;
	}
	int i;
	List<Vertex *>tmp;
	for(i=0;i<u->neighbor.num;i++) {
		tmp.Add(u->neighbor[i]);
	}
	for(i=u->face.num-1;i>=0;i--) {
		if(u->face[i]->HasVertex(v)) {
			delete(u->face[i]);
		}
	}
	for(i=u->face.num-1;i>=0;i--) {
		u->face[i]->ReplaceVertex(u,v);
	}
	delete u; 
	for(i=0;i<tmp.num;i++) {
		ComputeEdgeCostAtVertex(tmp[i]);
	}
}

void AddVertex(List<Vector> &vert){
	for(int i=0;i<vert.num;i++) {
		Vertex *v = new Vertex(vert[i],i);
	}
}
void AddFaces(List<tridata> &tri){
	for(int i=0;i<tri.num;i++) {
		Triangle *t=new Triangle(
					      vertices[tri[i].v[0]],
					      vertices[tri[i].v[1]],
					      vertices[tri[i].v[2]] );
		charts.push_back(new Chart(i,t));
	}
}

Vertex *MinimumCostEdge(){
	Vertex *mn=vertices[0];
	int k = -1;
	for(int i=0;i<vertices.num;i++) {
		if(!vertices[i]->onBoundary && vertices[i]->collapse && !vertices[i]->collapse->onBoundary && vertices[i]->objdist < mn->objdist) {
			mn = vertices[i];
			k = i;
		}
	}
	return mn;
}

void ProgressiveMesh(List<Vector> &vert, List<tridata> &tri, 
                     List<int> &map, List<int> &permutation, List<int> &chart_map, int &boundary_len )
{
	AddVertex(vert); 
	AddFaces(tri);
	
	chart_map.SetSize(triangles.num); 
	boundary_len = MergeCharts();
	for(int i=0;i<triangles.num;i++)
		chart_map[i] = triangles[i]->chart_id;
		
	ComputeAllEdgeCollapseCosts();
	permutation.SetSize(vertices.num);  
	map.SetSize(vertices.num);
	
	
	while(vertices.num > 0) {
		Vertex *mn = MinimumCostEdge();
		permutation[mn->id]=vertices.num-1;
		map[vertices.num-1] = (mn->collapse)?mn->collapse->id:-1;
		Collapse(mn,mn->collapse);
	}
	for(int i=0;i<map.num;i++) {
		map[i] = (map[i]==-1)?0:permutation[map[i]];
	}
}
