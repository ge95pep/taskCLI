#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include "utils.h"

using json = nlohmann::json;



class Task {
public:
    int id;
    std::string description;
    std::string status;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;

    // Default Constructor
    Task()
        : id(0), 
        description("No description yet"), 
        status("todo"),
        createdAt(std::chrono::system_clock::now()),
        updatedAt(std::chrono::system_clock::now()){}
    // Construct directly from JSON object
    Task(const json& jsondata)
        : id(jsondata.value("id", 0)),
        description(jsondata.value("description", "No description yet")),
        status(jsondata.value("status", "todo")),
        createdAt(parseTime(jsondata.value("createdAt", "1970-01-01T00:00:00Z"))),
        updatedAt(parseTime(jsondata.value("updatedAt", "1970-01-01T00:00:00Z"))){}

    void setID(int newID){
        id = newID;
    }
    void setDescription(std::string taskDescription){
        description = taskDescription;
    }
};

// GLOBAL variables
static const std::filesystem::path jsonfile_path = "~/.config/taskCLI/tasks.json";
static std::unordered_map<int, Task> tasksMap;
static const std::unordered_set<std::string> validStatuses = {
        "todo", "in-progress", "done"
    };
static json loaded_data;


void loadTasks(){
    std::ifstream jsonfile(jsonfile_path);
    if (jsonfile.is_open() && jsonfile >> loaded_data){
        tasksMap.clear();//Start fresh

        if (loaded_data.contains("tasks") && loaded_data["tasks"].is_array()){
            for(const auto& item:loaded_data["tasks"]){
                Task task(item);
                tasksMap[task.id] = task;
            }
        }
        jsonfile.close();
    }
    else {
        //create a new JSON file, and add an empty list tasks:[]
        std::cout << "No existing tasks.json found. Creating a new one." << std::endl;

        loaded_data["tasks"] = json::array(); //Add empty "tasks": []
        std::ofstream out(jsonfile_path);
        out << loaded_data.dump(4);
        out.close();
    }
}

void saveTasks(){
    json tempdata;
    tempdata["tasks"] = json::array();

    // iterate over tasksMap and write into jeson data
    for (const auto& [id, task]:tasksMap){
        json taskItem;
        taskItem["id"] = task.id;
        taskItem["description"] = task.description;
        taskItem["status"] = task.status;
        taskItem["createdAt"] = formatTime(task.createdAt);
        taskItem["updatedAt"] = formatTime(task.updatedAt);

        tempdata["tasks"].push_back(std::move(taskItem));
    }

    //write to file with pretty writing
    std::ofstream jsonfile(jsonfile_path, std::ios::out | std::ios::trunc);
    if (jsonfile.is_open()){
        jsonfile << tempdata.dump(4);
        jsonfile.close();
    }
    else{
        std::cerr << "Error: Could not open tasks.json for writing!\n";
    }
}

int getNextID(){
    if (tasksMap.empty()){
        return 0;
    }
    // Generate new taskID
    // currrently set as maxID + 1
    int maxID = 0;
    for (auto iter=tasksMap.begin(); iter != tasksMap.end(); ++iter){
        auto curr = iter->first;
        if (curr > maxID){
            maxID = curr;
        }
    }
    return maxID + 1;
}

void showhelp(){
    std::cout << "This is a Task Tracking project" << std::endl;
    std::cout << "Type in commands to start!" << std::endl;
    std::cout << "Usage:\n";
    std::cout << "   taskCLI add \"task description\"    Add a new task\n";
    std::cout << "   taskCLI list                       List all tasks\n";
    std::cout << "   taskCLI done <id>                  Mark task as done\n";
    std::cout << "   taskCLI delete <id>                Delete a task\n";
    std::cout << "Note: Task descriptions must be enclosed in double quotes." << std::endl;
}

void addTask(const std::string taskDescription){
    // create new Task item
    int newID = getNextID();
    Task newTask;
    newTask.setID(newID);
    newTask.setDescription(taskDescription);
    // update tasksMap
    tasksMap[newID] = newTask;
    // update jsonfile
    saveTasks();

    std::cout << "Task added successfully (ID: " << newID << ")" << std::endl;
}

void updateTask(int taskID,const std::string& newDescription){
    // check ID validation
    auto it = tasksMap.find(taskID);
    if (it == tasksMap.end()){
        std::cout << "Error: Task with ID " << taskID << " not found. Please check your input. \n";
        std::cout << "Use \"taskCLI list\" to list all valid tasks" << std::endl;
        return;
    }

    if (newDescription.empty()){
        std::cout << "Warning: No descripition provided, thus no change made" << std::endl;
    }

    // Actual change
    it->second.description = newDescription;
    it->second.updatedAt = std::chrono::system_clock::now();

    saveTasks();

    std::cout << "Task " << taskID << " updated successfully. \n"
              << "New description: \"" << newDescription << "\"" << std::endl;
}

void deleteTasks(const std::vector<int>& taskIDs){ 
    if (taskIDs.empty()){
        std::cout << "No tasks IDs provided." << std::endl;
        return;
    }

    std::vector<int> toDelete;
    std::vector<int> notFound;

    for (size_t id:taskIDs){
        if (tasksMap.find(id) == tasksMap.end()){
            notFound.push_back(id);
        } else {
            toDelete.push_back(id);
        }
    }
    // Early exit if nothing to deleted
    if (toDelete.empty()){
        std::cout << "None of the tasks exit. Please check your input." << std::endl;
        return; 
    }

    // Confirmation
    std::sort(toDelete.begin(), toDelete.end());
    for (int id:toDelete){
        std::cout << "\n   #" << id << "    \"" << tasksMap[id].description << "\"";
    }

    // Show notFound
    if (!notFound.empty()){
        std::cout << "\nNote: The following IDs were not found and will be ignored: ";
        for (auto id:notFound){
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Confirm deletion of " << toDelete.size() << " tasks? [y/N]: " << std::endl;

    std::string response;
    std::getline(std::cin, response);
    
    if (response != "y" && response != "Y"){
        std::cout << "Deletion canceled." << std::endl;
        return;
    }

    // Actual deletion
    for (auto id:toDelete){
        tasksMap.erase(id);
    }

    saveTasks();

    std::cout << "Successfully deleted " << toDelete.size() << " tasks." << std::endl;
}

void mark(int taskID, std::string trgtStatus){
    auto it = tasksMap.find(taskID);
    if (it == tasksMap.end()){
        std::cout << "Task with ID " << taskID << " not found." << std::endl;
        return;
    }

    //check if the trgtStatus is valid
    if (validStatuses.find(trgtStatus) == validStatuses.end()){
            std::cout << "Error: Invalid status. Use 'todo', 'in-progress' or 'done'." << std::endl;
            return;
        }
    
    it->second.status = trgtStatus;
    it->second.updatedAt = std::chrono::system_clock::now();
    saveTasks();
    std::cout << "Task " << taskID << " marked as " << trgtStatus << std::endl;
}


void listTasks(std::string filter){
    if (tasksMap.empty()){
        std::cout << "No tasks found." << "Type: taskCLI add \"your task description\" to add new task" << std::endl;
        return;
    }
    
    // collect all relevant IDs and sorting
    std::vector<int> sortedIDs;
    sortedIDs.reserve(tasksMap.size()); // avoid reallocation
    for (const auto& [id, task]:tasksMap){
        if (filter.empty() || task.status == filter){
            sortedIDs.push_back(id);
        }
    }
    std::sort(sortedIDs.begin(), sortedIDs.end());

    std::cout << "Tasks:\n";
    std::cout << "ID   Status          Description\n";
    std::cout << "---------------------------------------\n";

    for (size_t id:sortedIDs){
        std::cout << std::left 
        << std::setw(5) << id
        << std::setw(16) << tasksMap[id].status
        << tasksMap[id].description << std::endl;
    }
}


int main(int argc, char* argv[]){
    // check if there's valid command
    if (argc == 1){
        showhelp();
        return 0;
    }
    // read from JSON file
    loadTasks();

    // handle different command
    std::string command = argv[1];

    if (command == "add"){
        if (argc < 3) {
            std::cout << "Error: Missing task description." << std::endl;
            std::cout << "Usage: taskCLI add \"your task description\"" << std::endl;
            return 1;
        }
        // right now we only support one task addition at a time
        // TODO: multi-task addition support
        std::string taskDescription = argv[2];
        addTask(taskDescription);
    }
    else if (command == "update") {
        if (argc < 4) {
            std::cout << "Error: Missing parameters for update." << std::endl;
            std::cout << "Usage: taskCLI update <id> \"new task description\"" << std::endl;
            return 1;
        }
        int taskID = std::stoi(argv[2]);
        std::string newDescription = argv[3];
        updateTask(taskID, newDescription);
    }
    else if (command == "delete") {
        if (argc < 3) {
            std::cout << "Error: Missing taskID." << std::endl;
            std::cout << "Usage: taskCLI delete <id>" << std::endl;
            return 1;
        }
        std::vector<int> IDs;
        try{
            for (int i=2; i<argc; ++i){
                IDs.push_back(std::stoi(argv[i]));
            }
            deleteTasks(IDs);
        }catch(...){
            std::cout << "Error: Invalid task ID(s)" << std::endl;
        }
    }
    else if (command == "mark") {
        if (argc < 3){
            std::cout << "Error: Missing taskID or target status." << std::endl;
            std::cout << "Usage: taskCLI mark <id> <target status>" << std::endl;
            return 1;
        }
        int taskID = std::stoi(argv[2]);
        std::string trgtStatus = argv[3];
        mark(taskID, trgtStatus);
    }
    else if (command == "list") {
        std::string filter;

        if (argc > 2) {filter = argv[2];}
        
        listTasks(filter);
    }
    else {
        std::cout << "Unknown command: " << command << std::endl;
        showhelp();
    }
    return 0;
}