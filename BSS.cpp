#include <bits/stdc++.h>
using namespace std;
const int N = 3;

vector<vector<vector<string>>> process;
map<string, vector<int>> timestamp;
map<pair<int, string>, vector<int>> recv_A;
map<string, vector<int>> recv_B;
map<string, int> sender;
map<string, bool> marked;
vector<bool> isterminated(N, 0);
vector<bool> hasbegin(N, 0);
vector<vector<int>> clock_time(N,vector<int>(N,0));
map<int, queue<string>> delayed;
map<pair<int, int>, vector<int>> current_time;
vector<int>lastevent(N,0);

void run_process(int i);
vector<string> tokenize(string line);
bool can_accept(int i, int p, string msg);

vector<string> tokenize(string line)
{
    vector<string> tokens;

    // stringstream class check1
    stringstream check1(line);

    string intermediate;

    // Tokenizing w.r.t. space ' '
    while (getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
        // cout<<intermediate<<endl;
    }
    return tokens;
}

bool can_accept(int i, int p, string msg)
{
    vector<int> ts = timestamp[msg];
    bool flag = 1;
    if (clock_time[i][p] != ts[p] - 1)
        flag = 0;
    for (int k = 0; k < N; k++)
    {
        if (k == p)
            continue;
        if (clock_time[i][k] < ts[k])
        {
            flag = 0;
            break;
        }
    }
    return flag;
}

bool is_ahead(string m1, string m2)
{
    bool all_eq = true;
    for (int i = 0; i < N; i++)
    {
        if (timestamp[m2][i] < timestamp[m1][i])
            return false;
        if (timestamp[m2][i] > timestamp[m1][i])
            all_eq = false;
    }
    return !all_eq;
}

void run_process(int i)
{
    if (isterminated[i] )
        return;
    // vector<int> tmp_clock_time(N, 0);
    // clock_time[i] = tmp_clock_time;
    hasbegin[i] = 1;

    for (int j = lastevent[i]+1; j < process[i].size(); j++)
    {
        if (process[i][j][0] == "send")
        {
            if(process[i][j].size()!=2){
//                cout<<process[i][j][0]<<endl;
                cerr<<"ERROR: Syntax Error\n";
                exit(1);
            }
            clock_time[i][i]++;
            timestamp[process[i][j][1]] = clock_time[i];
            current_time[{i, j}] = clock_time[i];
            marked[process[i][j][1]] = 1;
            sender[process[i][j][1]] = i;
        }
        else if (process[i][j][0] == "recv_B")
        {
            if(process[i][j].size()!=3){
                cerr<<"ERROR: Syntax Error\n";
                exit(1);
            }
            current_time[{i, j}] = clock_time[i];

            int p = process[i][j][1][1] - '1'; // sender
            if (marked[process[i][j][2]] == 0)
            {
                lastevent[i]=j-1;
                run_process(p);
                return;
            }

            recv_B[process[i][j][2]] = clock_time[i];
            vector<int> ts = timestamp[process[i][j][2]];

            bool flag = can_accept(i, p, process[i][j][2]);
            if (flag == 0)
            {
                delayed[i].push(process[i][j][2]);
            }
            else
            {
                // update time
                for (int k = 0; k < N; k++)
                {
                    clock_time[i][k] = max(clock_time[i][k], ts[k]);
                }
                recv_A[{i, process[i][j][2]}] = clock_time[i];

                vector<pair<int, string>> tmp;
                // map<string,int> arrival_time;
                int ctr = 0;
                set<pair<int, string>> delayed_messages;
                while (delayed[i].empty() == 0)
                {
                    tmp.push_back({ctr, delayed[i].front()});
                    delayed_messages.insert({ctr, delayed[i].front()});
                    // arrival_time[delayed[i].front()]=ctr;
                    ctr++;
                    // string msg = delayed[i].front();
                    // p = sender[msg];
                    // flag = can_accept(i, p, msg);
                    // if (flag == 0)
                    // {
                    //     break;
                    // }
                    // ts = timestamp[msg];
                    // for (int k = 0; k < N; k++)
                    // {
                    //     clock_time[i][k] = max(clock_time[i][k], ts[k]);
                    // }
                    // recv_A[{i, msg}] = clock_time[i];
                    delayed[i].pop();
                }
                vector<vector<int>> adj(ctr);
                vector<int> in_deg(ctr);
                for (int i = 0; i < ctr; i++)
                {
                    for (int j = 0; j < ctr; j++)
                    {
                        if (is_ahead(tmp[i].second, tmp[j].second))
                        {
                            adj[i].push_back(j);
                            in_deg[j]++;
                        }
                    }
                }
                set<int> start_vertices;
                for (int i = 0; i < ctr; i++)
                {
                    if (in_deg[i] == 0)
                        start_vertices.insert(i);
                }
                while (!start_vertices.empty())
                {
                    int cur = *(start_vertices.begin());
                    string msg = tmp[cur].second;
                    p = sender[msg];
                    flag = can_accept(i, p, msg);
                    if (flag == 0)
                    {
                        break;
                    }
                    delayed_messages.erase({cur, msg});
                    for (auto x : adj[cur])
                    {
                        in_deg[x]--;
                        if (in_deg[x] == 0)
                            start_vertices.insert(x);
                    }
                    ts = timestamp[msg];
                    for (int k = 0; k < N; k++)
                    {
                        clock_time[i][k] = max(clock_time[i][k], ts[k]);
                    }
                    recv_A[{i, msg}] = clock_time[i];
                }
                for (auto x : delayed_messages)
                    delayed[i].push(x.second);
            }

        }
        else if (process[i][j][0] == "end")
        {
            if(process[i][j].size()!=3){
                cerr<<"ERROR: Syntax Error\n";
                exit(1);
            }
            if(!delayed[i].empty()){
                cerr<<"ERROR: some messages are not received\n";
                exit(1);
            }
            isterminated[process[i][j][2][1] - '1'] = 1;
            lastevent[i]=j;
        }
        else {
            cerr<<"ERROR: Syntax Error\n";
            exit(1);
        }
    }
    isterminated[i] = 1;
    lastevent[i]=process[i].size()-1;;
}

// Utility function to detect cycle in a directed graph
bool isCyclicUtil(vector<vector<int>>& adj, int u,
                  vector<bool>& visited, vector<bool>& recStack) {

    if (!visited[u]) {

        // Mark the current node as visited
        // and part of recursion stack
        visited[u] = true;
        recStack[u] = true;

        // Recur for all the vertices adjacent
        // to this vertex
        for (int x : adj[u]) {
            if (!visited[x] &&
                isCyclicUtil(adj, x, visited, recStack))
                return true;
            else if (recStack[x])
                return true;
        }
    }

    // Remove the vertex from recursion stack
    recStack[u] = false;
    return false;
}

// Function to detect cycle in a directed graph
bool isCyclic(vector<vector<int>>& adj, int V) {
    vector<bool> visited(V, false);
    vector<bool> recStack(V, false);

    // Call the recursive helper function to
    // detect cycle in different DFS trees
    for (int i = 0; i < V; i++) {
        if (!visited[i] &&
            isCyclicUtil(adj, i, visited, recStack))
            return true;
    }

    return false;
}


int main()
{
    freopen("output.txt", "w", stdout);
    std::ifstream file("input.txt");
    string line;

    vector<vector<string>> tmp;
    while (std::getline(file, line))
    {
        // cout<<line<<endl;
        if (line == "*")
        {
            // cout<<"*";
            process.push_back(tmp);
            tmp.erase(tmp.begin(), tmp.end());
            continue;
        }
        vector<string> v = tokenize(line);
        tmp.push_back(v);
    }
    file.close();
    // for(auto i:process){
    //     for(auto j:i){
    //         for(auto k:j){
    //             cout<<k<<endl;
    //         }
    //     }
    // }
//    for (int i = 0; i < process.size(); i++)
//    {
//        for (int j = 0; j < process[i].size(); j++)
//        {
//
//            if (process[i][j][0] == "begin" && j != 0)
//            {
//                cerr << "ERROR:events happen before process begins\n";
//            }
//        }
//    }
    set<string>sent;
    for(int i=0;i<process.size();++i){
        for(int j=0;j<process[i].size();j++){
            if(process[i][j][0]=="send")
                sent.insert(process[i][j][1]);
        }
    }

    for(int i=0;i<process.size();++i){
        for(int j=0;j<process[i].size();j++){
            if(process[i][j][0]=="recv_B"){
                string check=process[i][j][2];
                if(sent.find(check)==sent.end()){
                    cerr<<"wrong input: received message sent by no site";
                    exit(1);
                }
            }
        }
    }
    for(int i=0;i<process.size();i++){
        if(process[i].empty() || process[i][0][0]!="begin" || process[i][process[i].size()-1][0]!="end"){
            cerr<<"ERROR: process doesn't have correct begin or end events\n";
            exit(1);
        }
    }
    for(int i=0;i<process.size();i++){
        if(process[i][0].size()!=3){
            cerr<<"ERROR: Syntax Error\n";
            exit(1);
        }
    }



    int total_events=0;
    map<int,vector<string>> events_corresponding_to_numbers;
    vector<vector<int>> adj;
    for(int i=0;i<process.size();i++){
        for(int j=0;j<process[i].size();j++){
            if(process[i][j][0]!="begin" && process[i][j][0]!="end"){
                events_corresponding_to_numbers[total_events]=process[i][j];
                adj.push_back(vector<int>());
                if(j!=1) adj[total_events-1].push_back(total_events);
                total_events++;
            }
        }
    }
    for(auto x:events_corresponding_to_numbers){
        if(x.second[0]=="send"){
            for(auto y:events_corresponding_to_numbers){
                if(y.second[0]=="recv_B" && y.second[2]==x.second[1]){
                    adj[x.first].push_back(y.first);
                }
            }
        }
    }

    if(isCyclic(adj,total_events)){ cerr<<"ERROR: Events in the process are cyclic\n";
        exit(1);
    }

    set<int>remaining;
    for(int i=0;i<process.size();i++){
        remaining.insert(i);
    }
    while(!remaining.empty()){
        int t=*remaining.begin();
        if(isterminated[t]==1){
            remaining.erase(t);
            continue;
        }
        run_process(t);
    }
    // run_process(0);
    // run_process(1);
    // run_process(2);

    for (int i = 0; i < process.size(); i++)
    {
        for (int j = 0; j < process[i].size(); j++)
        {
            if (process[i][j][0] == "begin" || process[i][j][0] == "end")
            {
                for (auto s : process[i][j])
                {
                    cout << s << " ";
                }
                cout << endl;
            }
            if (process[i][j][0] == "send")
            {
                for (auto s : process[i][j])
                {
                    cout << s << " ";
                }
                cout<<"(";
                for (auto z : current_time[{i, j}])
                {
                    cout << z;
                }
                cout << ")\n";
            }
            if (process[i][j][0] == "recv_B")
            {
                for (auto s : process[i][j])
                {
                    cout << s << " ";
                }
                cout<<"(";
                for (auto z : current_time[{i, j}])
                {
                    cout << z;
                }
                cout << ")\n";
                cout << "recv_A: " << process[i][j][1] << " " << process[i][j][2] << " ";
                cout<<"(";
                for (auto z : recv_A[{i, process[i][j][2]}])
                {
                    cout << z;
                }
                cout <<")\n";
            }
        }
        cout << endl;
    }
    return 0;
}
