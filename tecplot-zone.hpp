#ifndef __TECPLOT_ZONE__
#define __TECPLOT_ZONE__

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#define __DEBUG_TECPLOT_ZONE__ true

class tecplot_zone{
    public:
        tecplot_zone();
        tecplot_zone(std::string name, size_t number_of_variables, size_t offset=0);
        void setName(std::string name);
        void setStrandID(size_t strandid);
        void setSolTime(double sol_time);
        void setNumberOfNodes(size_t number_of_nodes);
        void setNumberOfElements(size_t number_of_elements);
        void setZoneType(std::string zone_type);
        void setDataPacking(std::string dp_type);
        void setDataTypes(std::string dt_line);
        void setOffset(size_t offset);
        size_t getOffset();
        void addLine(std::string line);
        void addFooterLine(std::string line);
        std::string getName();
        size_t getStrandID();
        double getSolTime();
        size_t size();
        double getValue(size_t number_of_node, size_t number_of_variable);
        std::vector<double> getNode(size_t number_of_node);
        std::vector<double> getNode(size_t number_of_node, std::vector<size_t> number_of_variable);
        size_t getNumberOfNodes();
        size_t getNumberOfElements();
        bool isInitialised();
        double circ_dist(std::vector<double> p1, std::vector<double> p2);
        void setCoordColumns(std::vector<size_t> columns);
        size_t findNode(std::vector<double> coords, double epsilon);
        std::vector<double> getNodeCoords(size_t node);
    private:
        std::string name;
        size_t strandid=0;
        double solution_time = 0;
        size_t nodes = 0;
        size_t elements = 0;
        bool zone_initialised;
        std::string zone_type;
        std::string datapacking;
        std::vector<std::string> data_types;
        std::vector<std::vector<double>> contents;
        std::vector<std::vector<size_t>> footer;
        size_t offset;
        size_t number_of_variables;
        std::vector<size_t> coord_columns;
};

#endif