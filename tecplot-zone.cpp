#include "tecplot-zone.hpp"

tecplot_zone::tecplot_zone(){

}

tecplot_zone::tecplot_zone(std::string name, size_t number_of_variables){
    /*Defines new zone*/
    this->setName(name);
    this->number_of_variables = number_of_variables;
    this->zone_initialised = true;
}

bool tecplot_zone::isInitialised(){
    return this->zone_initialised;
}

void tecplot_zone::setName(std::string name){
    if(name.back()=='\n') name.pop_back();
    if(name.front()=='\"') name = name.substr(1);
    if(name.back()=='\"') name.pop_back();
    this->name = name;
}

void tecplot_zone::setStrandID(size_t strandid){
    this->strandid = strandid;
}

void tecplot_zone::setSolTime(double sol_time){
    this->solution_time = sol_time;
}

void tecplot_zone::setNumberOfNodes(size_t number_of_nodes){
    this->nodes = number_of_nodes;
}

void tecplot_zone::setNumberOfElements(size_t number_of_elements){
    this->elements = number_of_elements;
}

void tecplot_zone::setZoneType(std::string zone_type){
    this->zone_type = zone_type;
}

void tecplot_zone::setDataPacking(std::string dp_type){
    if(dp_type.back()=='\n') dp_type.pop_back();
    this->datapacking = dp_type;
}

size_t tecplot_zone::size(){
    //returns true number of nodes
    return this->contents.size();
}

void tecplot_zone::setDataTypes(std::string data_types){
    //iterator
    size_t i=0;
    //find first ' ' sign
    auto dt_sp = data_types.find(" ");
    //the 1 here is set due to '(' sign being at the begining of line
    size_t dt_bg = 1;
    //find all spaces in data types. Stop, if end of line was reached
    for(i=0; dt_sp!=std::string::npos; i++){
        //add data type to array
        this->data_types.push_back(
            data_types.substr(dt_bg, dt_sp-dt_bg)
        );
        //move current "cursor" past the found ' ' sign
        dt_bg = dt_sp+1;
        //find next ' ' sign
        dt_sp = data_types.find(" ", dt_bg);
    }
    //remove ')' sign from data types array
    if(this->data_types.back().compare(")")==0)
        this->data_types.pop_back();
}

void tecplot_zone::addLine(std::string line){
    //allocate memory for data line
    std::vector<double> data_line(this->number_of_variables, 0.);
    //iterator
    size_t i=0;
    //finishing argument for loop
    auto dt_end = std::string::npos;
    //find first ' ' sign
    auto dt_sp = line.find(" ", 1);
    size_t dt_bg = 1;
    //find all spaces in data. Stop, if end of line was reached
    for(i=0; dt_sp!=dt_end; i++){
        //add data to array
        data_line.at(i) =
            std::stod(line.substr(dt_bg, dt_sp-dt_bg));
        //move current "cursor" past the found ' ' sign
        dt_bg = dt_sp+1;
        //find next ' ' sign
        dt_sp = line.find(" ", dt_bg);
    }
    this->contents.push_back(data_line);
    std::vector<double>().swap(data_line);
}

void tecplot_zone::addFooterLine(std::string line){
    //allocate memory for footer line
    std::vector<size_t> footer_line(this->number_of_variables, 0.);
    //iterator
    size_t i=0;
    //finishing argument for loop
    auto dt_end = std::string::npos;
    //find first ' ' sign
    auto dt_sp = line.find(" ", 1);
    size_t dt_bg = 1;
    //find all spaces in data. Stop, if end of line was reached
    for(i=0; dt_sp!=dt_end; i++){
        //add data to array
        footer_line.at(i) =
            std::stod(line.substr(dt_bg, dt_sp-dt_bg));
        //move current "cursor" past the found ' ' sign
        dt_bg = dt_sp+1;
        //find next ' ' sign
        dt_sp = line.find(" ", dt_bg);
    }
    this->footer.push_back(footer_line);
}

double tecplot_zone::getValue(size_t number_of_node, size_t number_of_variable){
    if(number_of_node>this->size())
        throw std::length_error("getValue: Node ID\
            exceeds number of available nodes in zone!");
    if(number_of_variable>this->number_of_variables)
        throw std::length_error("getValue: Column ID\
            exceeds number of variables!");
    return this->contents.at(number_of_node).at(number_of_variable);
}

std::vector<double> tecplot_zone::getNode(size_t number_of_node){
    /* getNode(size_t, std::vector<size_t>)
       returns vector of node data
    */
    if(number_of_node>this->size())
        throw std::length_error("getNode: Node ID\
            exceeds number of available nodes in zone!");
    return this->contents.at(number_of_node);
}

std::vector<double> tecplot_zone::getNode(
    size_t number_of_node,
    std::vector<size_t> number_of_variable
){
    /* getNode(size_t, std::vector<size_t>)
       returns vector of node data from given variable indexes
    */
    //allocate memory for result
    std::vector<double> res;
    //add values from contents to result array
    for(size_t i=0; i<number_of_variable.size(); i++){
        if(number_of_variable.at(i)>this->number_of_variables)
            throw std::length_error("getNode: Column ID exceeds\
                number of variables!");
        res.push_back(this->getValue(
            number_of_node,
            number_of_variable.at(i)
        ));
    }
    return res;
}

size_t tecplot_zone::getNumberOfNodes(){
    return this->nodes;
}

size_t tecplot_zone::getNumberOfElements(){
    return this->elements;
}

std::string tecplot_zone::getName(){
    return this->name;
}

double tecplot_zone::getSolTime(){
    return this->solution_time;
}

size_t tecplot_zone::getStrandID(){
    return this->strandid;
}

void tecplot_zone::setCoordColumns(std::vector<size_t> columns){
    size_t i=0;
    for(i=0;i<columns.size(); i++)
        if(columns.at(i)>this->number_of_variables)
            throw std::length_error("setCoordColumns: Column ID\
                exceeds number of variables!");
    this->coord_columns = columns;
}

double tecplot_zone::circ_dist(std::vector<double> p1, std::vector<double> p2){
    size_t chk = (p1.size() < p2.size()) ? p1.size() : p2.size();
    double sum=0;
    for(chk; chk>=1; chk--)
        sum += pow(p2.at(chk-1)-p1.at(chk-1), 2.);
    return sqrt(sum);
}

std::vector<double> tecplot_zone::getNodeCoords(size_t node){
    return this->getNode(node, this->coord_columns);
}

size_t tecplot_zone::findNode(std::vector<double> coords, double epsilon=1e-6){
    size_t nodeID = 0;
    //if coordinate columns were not set, throw an exception
    if(this->coord_columns.size()==0)
        throw std::length_error("Coordinate columns not set! Please\
            specify columns containing node coordinates before\
            calling findPoint() function.");
    //array of found nodes' ID numbers
    std::vector<size_t> nodes;
    //array of distances to the original node
    std::vector<double> distances;
    //current node distance variable
    double cnode_distance;
    for(nodeID=0; nodeID<this->size(); nodeID++){
        //initial check
        if(fabs(coords.at(0)-this->contents.at(nodeID).at(coord_columns.at(0)))>epsilon) continue;
        //calculate distance from original point
        cnode_distance = this->circ_dist(coords, this->getNodeCoords(nodeID));
        //if distance is smaller than epsilon, add to list of found points
        if(cnode_distance<=epsilon){
            nodes.push_back(nodeID);
            distances.push_back(cnode_distance);
        }
    }
    //if there was no point found, throw exception
    if(nodes.size()==0) throw std::invalid_argument("findNode: It seems\
        that epsilon is too strict for given set of coordinates. No\
        points were found.");
    //if there was only one point found, return it
    if(nodes.size()==1) return nodes.at(0);
    //if there were multiple points found, return the closest one
    //(there is no point in making another if clause, as if runtime
    // was not caught by previous ifs, there must be more than one
    // nodes found)
    double min_dist=distances.at(0);
    double min_id = 0;
    for(nodeID=1; nodeID<nodes.size(); nodeID++){
        if(distances.at(nodeID)<min_dist){
            min_dist = distances.at(nodeID);
            min_id = nodeID;
        }
    }
    return nodes.at(min_id);
}