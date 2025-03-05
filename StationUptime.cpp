#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

// Merge a new interval defined by @start and @end to the existing collections of intervals
// - std::vector of @intervals are defined by pairs of starting and ending points
// - every 2 numbers in the std::vector represents a pair of starting and ending points
// * To minimize time consuming insert and delete operations, the intervals are not fully merged,
// * so consecutive intervals are kept as is, where the last intervals end is equal to the next intervals start
void merge(std::vector<int> &intervals,int start,int end){
    // Find where the start of the new interval falls into the current sequence of interval endpoints
    int i=0;
    while(i<intervals.size()&&start>intervals[i]){i++;}
    // Where the end of the new interval falls into the current sequence of interval endpoints
    int j=0;
    while(j<intervals.size()&&end>intervals[j]){j++;}
    if(i%2==0&&i==j){
        // No overlap with any of existing intervals
        intervals.insert(intervals.begin()+i,end);
        intervals.insert(intervals.begin()+i,start);
    }else{
        // When the new interval has overlap with existing intervals, modify the existing endpoints
        if(i%2==0){
            // Extend the next interval after @start forward
            intervals[i]=start;
        }
        if(j%2==0){
            // Extend the last interval before @end backwards
            intervals[j-1]=end;
        }
        if(j-i>1){
            // The space between intervals are filled
            for(int k=i;k<j-1;k++){
                if(k%2==1){
                    intervals[k]=intervals[k+1];
                }
            }
        }
    }
}

// Check if a string consists of only whitespace characters, for the use in processing the input
// in identifying break between stations list and charger availability reports
bool stringIsWhitespace(std::string &s){
    for(int i=0;i<s.size();i++){
        if(!std::isspace(s.at(i))){
            return false;
        }
    }
    return true;
} 

int main(int argc, char *argv[])
{
    std::ifstream fileInput;
    if(argc>1){
        fileInput.open(argv[1]);
    }
    std::istream &input=argc>1?fileInput:std::cin;
    std::string line;
    std::vector<int> stationInfo[100];
    // Header of [Stations]
    getline(input, line);
    int i=0;
    // Read the stations list into the std::vector stationInfo
    while(getline(input, line),!line.empty()&&!stringIsWhitespace(line)){
        std::stringstream s(line);
        int number;
        while(s >> number){
            stationInfo[i].push_back(number);
        }
        i++;
        if(input.eof()){
            // There must be an empty line between [Stations] and [Charger Availability Reports]
            // If not an error will be displayed
            std::cout<<"ERROR"<<std::endl;
            return 1;
        }
    }
    
    std::vector<int> chgrInfo[100];
    // Header of [Charger Availability Reports]
    getline(input, line);
    i=0;
    // Read the charger availability report into the std::vector chgrInfo
    while(getline(input, line),!line.empty()&&!stringIsWhitespace(line)){
        std::stringstream s(line);
        int id, stime, etime;
        std::string up;
        s >> id >> stime >> etime >> up;
        chgrInfo[i]={id,stime,etime,up.compare("true")==0?1:0};
        i++;
        if(input.eof()){break;}
    }
    
    std::map<int,std::vector<int>> chgrUpTime;
    // Organize the charger availability by the id of chargers
    // The first two numbers of a chgrUptime record are the earliest and latest reported times
    // All following numbers define the intervals when the charger is available, in pairs of 2
    for(int report=0;chgrInfo[report].size()>0;report++){
        if(chgrUpTime.find(chgrInfo[report][0])==chgrUpTime.end()){
            chgrUpTime[chgrInfo[report][0]]={chgrInfo[report][1],chgrInfo[report][2]};
        }else{
            if(chgrUpTime[chgrInfo[report][0]][0]>chgrInfo[report][1]){
                chgrUpTime[chgrInfo[report][0]][0]=chgrInfo[report][1];
            }
            if(chgrUpTime[chgrInfo[report][0]][1]<chgrInfo[report][2]){
                chgrUpTime[chgrInfo[report][0]][1]=chgrInfo[report][2];
            }
        }
        if(chgrInfo[report][3]){
            chgrUpTime[chgrInfo[report][0]].push_back(chgrInfo[report][1]);
            chgrUpTime[chgrInfo[report][0]].push_back(chgrInfo[report][2]);
        }
    }
    
    // Calculate the up time percentage for each station
    for(int i=0;stationInfo[i].size()!=0;i++){
        // Initialize the sequence of reportTime and upTime with the first pair of values
        // The sequences are defined by sets of intervals denoted by
        // numbers representing starting and ending points in pairs of 2
        std::vector<int> reportTime={chgrUpTime[stationInfo[i][1]][0],chgrUpTime[stationInfo[i][1]][1]};
        std::vector<int> upTime;
        // If no available time is reported, the upTime sequence has no values -- size() is 0
        if(chgrUpTime[stationInfo[i][1]].size()>2){
            upTime={chgrUpTime[stationInfo[i][1]][2],chgrUpTime[stationInfo[i][1]][3]};
        }
        // Repeatedly merge incoming intervals to existing list
        // so the final sequence represents the total reportedTime and upTime for a certain station
        // * To minimize time consuming insert and delete operations, the intervals are not fully merged,
        // * so consecutive intervals are kept as is
        // * where the last intervals end is equal to the next intervals start
        for(int j=1;j<stationInfo[i].size();j++){
            merge(reportTime,chgrUpTime[stationInfo[i][j]][0],chgrUpTime[stationInfo[i][j]][1]);
            for(int interval=2;interval<chgrUpTime[stationInfo[i][j]].size();interval+=2){
                merge(upTime,chgrUpTime[stationInfo[i][j]][interval],chgrUpTime[stationInfo[i][j]][interval+1]);
            }
        }
        // Add up all individual intervals to calculate the total reportedTime and total upTime
        int totalReportTime=0,totalUpTime=0;
        for(int interval=0;interval<reportTime.size();interval+=2){
            totalReportTime+=reportTime[interval+1]-reportTime[interval];
        }
        for(int interval=0;interval<upTime.size();interval+=2){
            totalUpTime+=upTime[interval+1]-upTime[interval];
        }
        // Calculate the percentage by dividing the totalUpTime by the totalReportTime
        std::cout<<stationInfo[i][0]<<" "<<(int)(100*(float)totalUpTime/(float)totalReportTime)<<std::endl;
    }
    
    return 0;
}
