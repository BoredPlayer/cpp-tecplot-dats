#include "tecplot-file.hpp"

tecplot_file::tecplot_file(std::string filename){
    std::ifstream file(filename);
    if(!file.is_open()) this->new_file = true;
    else this->new_file = false;
    file.close();
    this->filename = filename;
    this->loadFile();
}

void tecplot_file::switchAllFlags(){
    this->variables_flag=false;
    this->zone_header_flag = false;
    this->data_type_flag = false;
    this->data_flag = false;
    this->footer_flag = false;
}

char tecplot_file::decodeDataType(std::string line){
    if(line.find("TITLE")!=std::string::npos) return __TITLE__;

    if(line.find("VARIABLES")!=std::string::npos){
        this->switchAllFlags();
        this->variables_flag=true;
        return __VARIABLES_INIT__;
    }

    if(line.find("ZONE T=")!=std::string::npos){
        this->switchAllFlags();
        this->zone_header_flag = true;
        return __ZONE_HEADER__;
    }

    //this if clause (standard variables) must be AFTER zone header
    if(this->variables_flag){
        return __VARIABLES__;
    }

    if(line.find("DT=")!=std::string::npos){
        this->switchAllFlags();
        this->data_type_flag = true;
        return __DATA_TYPE__;
    }

    if(this->zone_header_flag) return __ZONE_HEADER__;

    if(this->data_type_flag){
        this->switchAllFlags();
        this->data_flag = true;
        return __DATA__;
    }
    if(this->zone.size()>0){
        if(this->zone.back().size()==this->zone.back().getNumberOfNodes() && this->zone.back().size()!=0){
            //std::cout<<"New zone!"<<std::endl;
            this->switchAllFlags();
            this->footer_flag = true;
            return __FOOTER__;
        }
    }
    if(this->data_flag)return __DATA__;
    return __UKNOWN__;
}

void tecplot_file::unpack_header(tecplot_zone* zone, std::string line){
    #if(__DEBUG_TECPLOT_FILE__)
    std::cout<<"Unpacking header line: "<<line<<std::endl;
    #endif
    size_t cursor = 0;
    double dtarget = 0.;
    size_t starget = 0;
    if(!(*zone).isInitialised()){
        cursor = line.find("=");
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Initialising new zone"<<std::endl;
        #endif
        (*zone) = tecplot_zone(line.substr(cursor+1), this->variables.size(), 0);
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Zone initialised"<<std::endl;
        std::cout<<"Zone name: "<<(*zone).getName()<<std::endl;
        #endif
        return;
    }
    if(line.find("STRANDID")!=std::string::npos){
        cursor=line.find("STRANDID")+9;
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Writing StrandID"<<std::endl;
        #endif
        starget = std::stoll(line.substr(cursor, line.find(",", cursor)));
        (*zone).setStrandID(starget);
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Zone StrandID: "<<(*zone).getStrandID()<<std::endl;
        #endif
    }
    if(line.find("SOLUTIONTIME")!=std::string::npos){
        cursor=line.find("SOLUTIONTIME")+13;
        dtarget = std::stod(line.substr(cursor));
        (*zone).setSolTime(dtarget);
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Zone Solution Time: "<<(*zone).getSolTime()<<std::endl;
        #endif
        return;
    }
    if(line.find("Nodes")!=std::string::npos){
        cursor=line.find("Nodes")+6;
        starget = std::stoll(line.substr(cursor, line.find(",", cursor)-cursor));
        (*zone).setNumberOfNodes(starget);
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Zone number of nodes: "<<(*zone).getNumberOfNodes()<<std::endl;
        #endif
    }
    if(line.find("Elements")!=std::string::npos){
        cursor=line.find("Elements")+9;
        starget = std::stoll(line.substr(cursor, line.find(",", cursor)-cursor));
        (*zone).setNumberOfElements(starget);
        #if(__DEBUG_TECPLOT_FILE__)
        std::cout<<"Zone number of Elements: "<<(*zone).getNumberOfElements()<<std::endl;
        #endif
    }

    if(line.find("ZONETYPE")!=std::string::npos){
        cursor=line.find("ZONETYPE")+10;
        (*zone).setZoneType(line.substr(cursor, line.size()-cursor-2));
        return;
    }
    if(line.find("DATAPACKING")!=std::string::npos){
        cursor = line.find("=")+1;
        (*zone).setDataPacking(line.substr(cursor));
        return;
    }
    if(line.find("DT=(")!=std::string::npos){
        (*zone).setDataTypes(line);
        return;
    }
    #if(__DEBUG_TECPLOT_FILE__)
    std::cout<<"Could not read line: \""<<line<<"\"\r\n";
    #endif
}

void tecplot_file::loadFile(){
    std::ifstream file(this->filename);
    std::string line;
    tecplot_zone zone;
    size_t cursor;
    char line_type;
    bool new_zone = false;
    size_t offset=0;

    while(file){
        std::getline(file, line);
        line_type = this->decodeDataType(line);
        if(line_type==__TITLE__){
            #if(__DEBUG_TECPLOT_FILE__)
            std::cout<<"Setting title"<<std::endl;
            #endif
            cursor = line.find("= ");
            this->setTitle(line.substr(cursor+2));
            continue;
        }
        if(line_type==__VARIABLES_INIT__){
            cursor = line.find("= ");
            this->addVariable(line.substr(cursor+2));
            continue;
        }
        if(line_type==__VARIABLES__){
            cursor = 0;
            this->addVariable(line);
            continue;
        }
        if(line_type==__ZONE_HEADER__ || line_type==__DATA_TYPE__){
            this->unpack_header(&zone, line);
            #if(__DEBUG_TECPLOT_FILE__)
            std::cout<<"Received new info on zone."<<std::endl;
            #endif
            new_zone=true;
            continue;
        }
        if(line_type==__DATA__){
            //std::cout<<"New data line."<<std::endl;
            if(new_zone){
                zone.setOffset(offset);
                this->zone.push_back(zone);
                zone = tecplot_zone();
                new_zone = false;
                #if(__DEBUG_TECPLOT_FILE__)
                std::cout<<"Loading data."<<std::endl;
                #endif
            }
            this->zone.back().addLine(line);
            continue;
        }
        if(line_type==__FOOTER__){
            this->zone.back().addFooterLine(line);
        }
        std::fflush(stdout);
    }
    #if(__DEBUG_TECPLOT_FILE__)
    std::cout<<"Data and footer loaded"<<std::endl;
    std::cout<<"Size of the zone: "<<this->zone.back().size()<<std::endl;
    #endif
    //save last read zone
    //this->zone.push_back(zone);
}

void tecplot_file::addVariable(std::string variable_name){
    if(variable_name.back()=='\n')
        variable_name.pop_back();
    if(variable_name.front()=='\"')
        variable_name = variable_name.substr(1);
    if(variable_name.back()=='\"')
        variable_name.pop_back();
    #if(__DEBUG_TECPLOT_FILE__)
    std::cout<<"Adding variable: "<<variable_name<<std::endl;
    #endif
    this->variables.push_back(variable_name);
}

void tecplot_file::setTitle(std::string title){
    if(title.back()=='\"')
        title.pop_back();
    if(title.front()=='\"')
        title = title.substr(1);
    if(title.back()=='\"')
        title.pop_back();
    this->title = title;
}

size_t tecplot_file::size(){
    return this->zone.size();
}

std::vector<double> tecplot_file::getNode(size_t zoneID, size_t node){
    if(zoneID>=this->size())
        throw std::length_error("getNode: Zone ID\
                exceeds number of zones!");
    return this->zone.at(zoneID).getNode(node);
}

std::vector<double> tecplot_file::getNode(
    size_t zoneID,
    size_t node,
    std::vector<size_t> columns
){
    size_t i=0;
    if(zoneID>=this->size())
        throw std::length_error("getNode: Zone ID\
            exceeds number of zones!");
    for(i=0; i<columns.size(); i++)
        if(columns.at(i)>this->variables.size())
            throw std::length_error("getNode: Column ID\
                exceeds number of variables!");
    return this->zone.at(zoneID).getNode(node+zone.at(i).getOffset(), columns);
}

std::vector<double> tecplot_file::getNode(
    size_t node
){
    size_t i = 0;
    for(i=0; i<this->size(); i++){
        if(this->zone.at(i).size()<node) continue;
        return getNode(node);
    }
    throw std::length_error("getNode: Node ID exceeds total number\
        of nodes in zones!");
}

size_t tecplot_file::firstZoneID(){
    return 0;
}

size_t tecplot_file::lastZoneID(){
    return this->size();
}

tecplot_zone tecplot_file::firstZone(){
    return this->zone.at(0);
}

tecplot_zone tecplot_file::lastZone(){
    return this->zone.back();
}

void tecplot_file::setCoordColumns(std::vector<size_t> columns){
    size_t i = 0;
    for(i=0; i<this->size(); i++){
        this->zone.at(i).setCoordColumns(columns);
    }
}

std::vector<double> tecplot_file::findNodeCoords(std::vector<double> coords, double epsilon){
    size_t i=0;
    size_t node = 0;
    for(i=0; i<this->size(); i++){
        try{
            node = this->zone.at(i).findNode(coords, epsilon);
        }
        catch(std::exception& exc){
            #if(__DEBUG_TECPLOT_FILE__)
            std::cout<<"Node not found in zone: "<<i<<std::endl;
            std::cout<<"Exception code:\n"<<exc.what()<<std::endl;
            #endif
            continue;
        }
        return this->zone.at(i).getNodeCoords(node);
    }
    throw std::invalid_argument("findNodeCoords: No point found!");
}

size_t tecplot_file::findNode(std::vector<double> coords, double epsilon){
    size_t i=0;
    for(i=0; i<this->size(); i++){
        try{
            return this->zone.at(i).findNode(coords, epsilon);
        }
        catch(std::exception& exc){
            #if(__DEBUG_TECPLOT_FILE__)
            std::cout<<"Node not found in zone: "<<i<<std::endl;
            std::cout<<"Exception code:\n\""<<exc.what()<<"\""<<std::endl;
            #endif
            continue;
        }
    }
    throw std::invalid_argument("findNodeCoords: No point found!");
}