#include <iostream>
#include <unordered_map>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <math.h> 

using namespace std;
const double dampingFactor = 0.15;
const double convergencyCoef = 0.000001;
const int iterationCount = 100;
const string fileName = "Pages.txt";

void addEdge(vector<int> adj[], int u, int v)
{
	adj[u].push_back(v);
}

unordered_map<int, double> pageRank(vector<int>* adj, int V, double dampingFactor, unordered_map<int, double> currRanks, unordered_map<int, string>pagesMapping)
{
	unordered_map<int, double> ranks;
	for (int u = 0; u < V; u++)
	{
		if (adj[u].empty())
		{
			ranks[u] = 1 / V;
		}
		else {
			for (auto neighbor : adj[u])
			{
				if (ranks.find(neighbor) == ranks.end())
				{
					ranks[neighbor] = (1 - dampingFactor) / V + dampingFactor * (currRanks[u] / adj[u].size());//add weight
						//ranks[neighbor] = currRanks[u] / adj[u].size(); //  naive
				}
				else
				{
					if (adj[u].size() != 0)
					{
						ranks[neighbor] += dampingFactor * (currRanks[u] / adj[u].size());
						// ranks[neighbor] += currRanks[u] / adj[u].size();
					}
				}
			}
		}
	}

	double sum = 0;
	for (auto entry : ranks)
	{
		cout << "element: " << pagesMapping[entry.first] << " weight: " << entry.second << "\n";
		sum += entry.second;
	}
	cout << "all elements' weight adds up to: " << sum << " end of iteration\n";
	return ranks;
}

bool checkForConvergence(unordered_map<int, double>prevRanks, unordered_map<int, double> currRanks, double convergencyCoef)
{
	//check the first and the last one
	double epsFirst = prevRanks[0] - currRanks[0];
	int last = prevRanks.size() - 1;
	double epsLast = prevRanks[last] - currRanks[last];
	if ((sqrt(epsFirst * epsFirst) > convergencyCoef) || (sqrt(epsLast * epsLast) > convergencyCoef))
	{
		return false;
	}
	
	cout << "converged";
	return true;
}

void handleSinks(vector<int>* adj, int V)
{
	for (int u = 0; u < V; u++)
	{
		if (adj[u].empty())
		{
			for (int i = 0; i < V; i++)
			{
				if (u != i)
					addEdge(adj, u, i);
			}
		}
	}
}

#pragma region HELPERS

vector<string> stringSplit(string delimiter, string s)
{
	vector<string> result;
	size_t pos = 0;
	string token;
	if (s.find(delimiter) == string::npos)
	{
		result.push_back(s);
		return result;
	}

	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		result.push_back(token);
		s.erase(0, pos + delimiter.length());
	}

	token = s.substr(0);
	result.push_back(token);

	return result;
}

vector<int>* readPages(unordered_map<int, string>& mapping, int& size)
{
	int graphSize = 100000;

	vector<int>* pagesGraph = new vector<int>[graphSize];
	unordered_map<string, int> pagesMapping;
	static int currPage = 0;
	string line;
	ifstream pages(fileName);
	if (pages.is_open()) {
		while (getline(pages, line)) {
			int index = line.find('-');
			string page = line.substr(0, index - 1);

			if (pagesMapping.find(page) == pagesMapping.end())
			{
				mapping[currPage] = page;
				pagesMapping[page] = currPage++;
			}

			string pageLinks = line.substr(index + 2);

			vector<string> neighbors = stringSplit(", ", pageLinks);

			for (auto neighbor : neighbors) {
				if (pagesMapping.find(neighbor) == pagesMapping.end())
				{
					mapping[currPage] = neighbor;
					pagesMapping[neighbor] = currPage++;
				}

				addEdge(pagesGraph, pagesMapping[page], pagesMapping[neighbor]);
			}
			//cout << pageLinks << '\n';
		}
		pages.close();
		size = currPage;
	}

	return pagesGraph;
}

#pragma endregion

int main()
{
	int V;//pages, page count and connections between them are all taken from the file Pages.txt, this is done in order to test easier the algorithm
	unordered_map<int, string> pagesMapping;
	vector<int>* graph = readPages(pagesMapping, V);

	unordered_map<int, double>ranks;
	handleSinks(graph, V);

	for (int i = 0; i < V; i++)
	{
		ranks[i] = 1.00 / (V);
	}

	for (int i = 0; i < iterationCount; i++)
	{
		cout << "Iteration: " << i + 1 << '\n';
		auto currRank = pageRank(graph, V, dampingFactor, ranks, pagesMapping);
		if (checkForConvergence(ranks, currRank, convergencyCoef))
		{
			//ranks have converged so we stop iterating
			break;
		}

		ranks = currRank;
		cout << "\n";
	}

	return 0;
}
