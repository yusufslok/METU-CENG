#include "MeshGraph.h"
#include "BinaryHeap.h"

// For printing
#include <fstream>
#include <iostream>
#include <sstream>

MeshGraph::MeshGraph(const std::vector<Double3>& vertexPositions,
                     const std::vector<IdPair>& edges)
{
    // TODO:
    for(int i=0;i<vertexPositions.size();i++){
        struct Vertex v;
        v.position3D = vertexPositions[i];
        v.id = i;
        vertices.push_back(v);
    }
    for(int j=0;j<vertexPositions.size();j++){
        std::list<Vertex*> list ;
        for(int k=0;k<edges.size();k++){
            if(vertices[j].id == edges[k].vertexId0){
                list.push_back(&vertices[edges[k].vertexId1]);
            }
            if(vertices[j].id == edges[k].vertexId1){
                list.push_back(&vertices[edges[k].vertexId0]);
            }
        }
        adjList.push_back(list);
    }
}

double MeshGraph::AverageDistanceBetweenVertices() const
{
    // TODO:
    double sum=0;
    int n=0;
    for(int i=0;i<adjList.size();i++){
        n+=adjList[i].size();
        for(int j=0;j<adjList[i].size();j++){
            std::list<Vertex*>::const_iterator it = adjList[i].begin();
            for(int k=0;k<j;k++){
                it++;
            }
            sum += Double3::Distance(vertices[i].position3D, (*it)->position3D);
        }
    }
    return sum/n;
}

double MeshGraph::AverageEdgePerVertex() const
{
    // TODO:
    double result;
    result = MeshGraph::TotalEdgeCount();
    result /= MeshGraph::TotalVertexCount();
    return result;
}

int MeshGraph::TotalVertexCount() const
{
    // TODO:
    return vertices.size();
}

int MeshGraph::TotalEdgeCount() const
{
    // TODO:
    int n=0;
    for(int i=0;i<adjList.size();i++){
        n+=adjList[i].size();}
        return n/2;
}

int MeshGraph::VertexEdgeCount(int vertexId) const
{
    // TODO:
    if(vertexId >= adjList.size()) return -1;
    return adjList[vertexId].size();
}

void MeshGraph::ImmediateNeighbours(std::vector<int>& outVertexIds,
                                    int vertexId) const
{
    // TODO:
    outVertexIds.resize(0);
    if(vertexId < vertices.size() and vertexId >=0){
        std::list<Vertex*>::const_iterator it = adjList[vertexId].begin();
        for(int k=0;k<adjList[vertexId].size();k++){
            outVertexIds.push_back((*it)->id);
            it++;
        }
    }
    
}

void MeshGraph::PaintInBetweenVertex(std::vector<Color>& outputColorAllVertex,
                                     int vertexIdFrom, int vertexIdTo,
                                     const Color& color) const
{
    // TODO:
    class BinaryHeap heap;
    std::vector<double> distances(vertices.size(),-1);
    std::vector<int> marks(vertices.size(),-2);
    distances[vertexIdFrom] = 0;
    marks[vertexIdFrom] = -1;
    for(int i=0;i<vertices.size();i++){
    heap.BinaryHeap::Add(vertices[i].id, INFINITY);
    }
    heap.BinaryHeap::ChangePriority(vertexIdFrom, 0);
    while(heap.HeapSize() != 0){
        int id;
        double weight;
        heap.PopHeap(id,weight);
        std::list<Vertex*>::const_iterator it = adjList[id].begin();
        for(int i=0;i<adjList[id].size();i++,it++){
            double new_distance = 0;
            new_distance = distances[id] + Double3::Distance(vertices[id].position3D, (*it)->position3D);
            if(distances[(*it)->id] < 0 || new_distance < distances[(*it)->id]){
                heap.BinaryHeap::ChangePriority((*it)->id, new_distance);
                distances[(*it)->id] = new_distance;
                marks[(*it)->id] = id;
            }
        }
    }
    for(int j=0;j<vertices.size();j++){
        struct Color c;
        c.r = 0;
        c.g =  0;
        c.b = 0;
        outputColorAllVertex.push_back(c);
    }
    
    int k = vertexIdTo;
    while(true){
       outputColorAllVertex[k] = color;
       if(marks[k] == -1){
           break;
       }
       k = marks[k];
    }
}

void MeshGraph::number(int maxDepth, int curDepth, int &num, int vertexId, std::vector<std::list<Vertex*> > adjList, std::vector<int> &markss) const{
    if(markss[vertexId] >= 0){
        if(markss[vertexId] > curDepth){
        markss[vertexId] = curDepth;
        num++;
        }
        else
            return;
    }
    if(maxDepth == curDepth) {
        if(markss[vertexId] < 0){
        markss[vertexId] = curDepth;
        num++;}
        return;
    }
    if(markss[vertexId] < 0){
    num++;}
    markss[vertexId] = curDepth;
    std::list<Vertex*>::const_iterator it = adjList[vertexId].begin();
    curDepth++;
    for(int i=0;i<adjList[vertexId].size();i++,it++){
        number(maxDepth, curDepth, num, (*it)->id, adjList, markss);
    }
}


void MeshGraph::PaintInRangeGeodesic(std::vector<Color>& outputColorAllVertex,
                                    int vertexId, const Color& color,
                                    int maxDepth, FilterType type,
                                    double alpha) const
{
    // TODO:
    outputColorAllVertex.resize(0);
    for(int j=0;j<vertices.size();j++){
        struct Color c;
        c.r = 0;
        c.g =  0;
        c.b = 0;
        outputColorAllVertex.push_back(c);
    }
    class BinaryHeap heap;
    std::vector<double> distances(vertices.size(),-1);
    std::vector<int> marks(vertices.size(),-2);
    distances[vertexId] = 0;
    if(type == FILTER_BOX){
                    if(-alpha <= 0 and 0 <= alpha){
                        outputColorAllVertex[vertexId] = color;
                    }
                }
                else if(type == FILTER_GAUSSIAN){
                    outputColorAllVertex[vertexId] = color;
                    outputColorAllVertex[vertexId].r *= exp(-pow(0,2)/pow(alpha,2));
                    outputColorAllVertex[vertexId].g *= exp(-pow(0,2)/pow(alpha,2));
                    outputColorAllVertex[vertexId].b *= exp(-pow(0,2)/pow(alpha,2));
                }
    marks[vertexId] = 1;
    for(int i=0;i<vertices.size();i++){
    heap.BinaryHeap::Add(vertices[i].id, INFINITY);
    }
    heap.BinaryHeap::ChangePriority(vertexId, 0);
    int depth =0;
    int num=0;
    std::vector<int> markss(vertices.size(),-1);
    number(maxDepth, 0, num, vertexId, adjList, markss);
    for(int k=0;k<vertices.size();k++){
        int id;
        double weight;
        heap.PopHeap(id,weight);
        std::list<Vertex*>::const_iterator it = adjList[id].begin();
        std::list<Vertex*>::const_iterator at = adjList[id].begin();
        std::list<int> listt;
        for(int j=0;j<adjList[id].size();at++,j++){
            listt.push_back((*at)->id);
        }
        listt.std::list<int>::sort();
        std::list<int>::const_iterator ut = listt.begin();
        for(int j=0;j<adjList[id].size();j++,ut++){
            std::list<Vertex*>::const_iterator at = adjList[id].begin();
            for(int i=0;i<adjList[id].size();i++,at++){
                if((*ut) == (*at)->id and marks[(*at)->id] < 0){
                    double new_distance = 0;
                    new_distance = distances[id] + Double3::Distance(vertices[id].position3D, (*at)->position3D);
                    depth++;
                    heap.BinaryHeap::ChangePriority((*at)->id, depth);
                    if(markss[(*at)->id] >= 0){
                        if(distances[(*at)->id] < 0 || new_distance < distances[(*at)->id])
                            distances[(*at)->id] = new_distance;
                        if(marks[(*at)->id] < 0){
                            marks[(*at)->id] = depth;
                        if(type == FILTER_BOX){
                            if(-alpha <= distances[(*at)->id] and distances[(*at)->id] <= alpha){
                                outputColorAllVertex[(*at)->id] = color;
                            }
                        }
                        else if(type == FILTER_GAUSSIAN){
                    outputColorAllVertex[(*at)->id] = color;
                    outputColorAllVertex[(*at)->id].r *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                    outputColorAllVertex[(*at)->id].g *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                    outputColorAllVertex[(*at)->id].b *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                            
                        }
                    }
                }
                }
            }
        }
    }
}

void MeshGraph::PaintInRangeEuclidian(std::vector<Color>& outputColorAllVertex,
                                      int vertexId, const Color& color,
                                      int maxDepth, FilterType type,
                                      double alpha) const
{
    // TODO:
    outputColorAllVertex.resize(0);
    for(int j=0;j<vertices.size();j++){
        struct Color c;
        c.r = 0;
        c.g =  0;
        c.b = 0;
        outputColorAllVertex.push_back(c);
    }
    class BinaryHeap heap;
    std::vector<double> distances(vertices.size(),-1);
    std::vector<int> marks(vertices.size(),-2);
    distances[vertexId] = 0;
    if(type == FILTER_BOX){
                    if(-alpha <= 0 and 0 <= alpha){
                        outputColorAllVertex[vertexId] = color;
                    }
                }
                else if(type == FILTER_GAUSSIAN){
                    outputColorAllVertex[vertexId] = color;
                    outputColorAllVertex[vertexId].r *= exp(-pow(0,2)/pow(alpha,2));
                    outputColorAllVertex[vertexId].g *= exp(-pow(0,2)/pow(alpha,2));
                    outputColorAllVertex[vertexId].b *= exp(-pow(0,2)/pow(alpha,2));
                }
    marks[vertexId] = 1;
    for(int i=0;i<vertices.size();i++){
    heap.BinaryHeap::Add(vertices[i].id, INFINITY);
    }
    heap.BinaryHeap::ChangePriority(vertexId, 0);
    int depth =0;
    int num=0;
    std::vector<int> markss(vertices.size(),-1);
    number(maxDepth, 0, num, vertexId, adjList, markss);
    for(int k=0;k<vertices.size();k++){
        int id;
        double weight;
        heap.PopHeap(id,weight);
        std::list<Vertex*>::const_iterator it = adjList[id].begin();
        std::list<Vertex*>::const_iterator at = adjList[id].begin();
        std::list<int> listt;
        for(int j=0;j<adjList[id].size();at++,j++){
            listt.push_back((*at)->id);
        }
        listt.std::list<int>::sort();
        std::list<int>::const_iterator ut = listt.begin();
        for(int j=0;j<adjList[id].size();j++,ut++){
            std::list<Vertex*>::const_iterator at = adjList[id].begin();
            for(int i=0;i<adjList[id].size();i++,at++){
                if((*ut) == (*at)->id and marks[(*at)->id] < 0){
                    double new_distance = 0;
                    new_distance = Double3::Distance(vertices[vertexId].position3D, (*at)->position3D);
                    depth++;
                    heap.BinaryHeap::ChangePriority((*at)->id, depth);
                    if(markss[(*at)->id] >= 0){
                        if(distances[(*at)->id] < 0)
                            distances[(*at)->id] = new_distance;
                        if(marks[(*at)->id] < 0){
                            marks[(*at)->id] = depth;
                        if(type == FILTER_BOX){
                            if(-alpha <= distances[(*at)->id] and distances[(*at)->id] <= alpha){
                                outputColorAllVertex[(*at)->id] = color;
                            }
                        }
                        else if(type == FILTER_GAUSSIAN){
                    outputColorAllVertex[(*at)->id] = color;
                    outputColorAllVertex[(*at)->id].r *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                    outputColorAllVertex[(*at)->id].g *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                    outputColorAllVertex[(*at)->id].b *= exp(-pow(distances[(*at)->id],2)/pow(alpha,2));
                            
                        }
                    }
                }
                }
            }
        }
    }
}

void MeshGraph::WriteColorToFile(const std::vector<Color>& colors,
                                 const std::string& fileName)
{
    // IMPLEMENTED
    std::stringstream s;
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        int r = static_cast<int>(colors[i].r);
        int g = static_cast<int>(colors[i].g);
        int b = static_cast<int>(colors[i].b);

        s << r << ", " << g << ", " << b << "\n";
    }
    std::ofstream f(fileName.c_str());
    f << s.str();
}

void MeshGraph::PrintColorToStdOut(const std::vector<Color>& colors)
{
    // IMPLEMENTED
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        std::cout << static_cast<int>(colors[i].r) << ", "
                  << static_cast<int>(colors[i].g) << ", "
                  << static_cast<int>(colors[i].b) << "\n";
    }
}