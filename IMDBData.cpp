// ITP 365 Spring 2018
// HW7 - IMDB Graph
// Name: Yuzhi Lu
// Email: yuzhilu@usc.edu
// Platform: Windows
//
// IDMBData.cpp - Implements IMDBData interface

#include "IMDBData.h"
#include <fstream>
#include <iostream>
#include <queue>

// Do for Part 1
// Function: Default Constructor
// Purpose: Loads in the actor/movie file specified,
// and populates the data into the appropriate hash maps.
// Also constructs the graph with actor relationships.
// Input: Name of file to load (by reference)
// Returns: Nothing
IMDBData::IMDBData(const std::string& fileName)
{
	//Generate the actor to movies map	
	std::ifstream input(fileName);
	std::string actor;
	std::vector<std::string> movie;

	if (input.is_open())
	{
		//read the first line as actor name
		std::getline(input, actor);

		while (!input.eof())
		{
			std::string temp;
			std::getline(input, temp);
			if (!temp.empty())
			{
				//read actor name
				if (temp.find("|") == std::string::npos)
				{
					// TODO: Generate the movie to actors map

					reverseMap(actor, mActorsToMoviesMap[actor]);
					//set actor name
					actor = temp;
					//create a new item in ActorsToMovies map
					mActorsToMoviesMap[actor] = movie;
				}
				// read movie name
				else
				{
					//get rid of |
					temp = temp.substr(1);
					//pust the movie into the vector in map
					mActorsToMoviesMap[actor].push_back(temp);
				}
			}
		}//end of reading the file
	}//end of file is open

}

// Do for Part 1
// Function: reverseMap
// Purpose: This is a helper function that is used to generate
// the reverse map (movies to actors)
// Input: Name of actor (by reference), vector of their movies (by reference)
// Returns: Nothing
void IMDBData::reverseMap(const std::string& actorName, const std::vector<std::string>& movies)
{
	for (std::string movieName : movies)
	{
		//check if movie is already in the hash map
		auto itr = mMoviesToActorsMap.find(movieName);
		//if already in the map
		if (itr != mMoviesToActorsMap.end())
		{
			itr->second.push_back(actorName);
		}
		//if not in the map
		else
		{
			std::vector<std::string> actors;
			actors.push_back(actorName);
			mMoviesToActorsMap[movieName] = actors;
		}
	}

}

// Do for Part 1
// Function: getMoviesFromActor
// Purpose: Given an actor's name, returns a vector containing their movies
// Input: Name of actor (by reference)
// Returns: Vector of movies (by reference)
std::vector<std::string>& IMDBData::getMoviesFromActor(const std::string& actorName)
{
	//if the actor name can be found
	if (mActorsToMoviesMap.find(actorName) != mActorsToMoviesMap.end())
	{
		return mActorsToMoviesMap[actorName];
	}
	return sEmptyVector;
}

// Do for Part 1
// Function: getActorsFromMovie
// Purpose: Given a movie's name, returns the name of its actors
// Input: Name of movie (by reference)
// Returns: Vector of actors (by reference)
std::vector<std::string>& IMDBData::getActorsFromMovie(const std::string& movieName)
{
	//if the movie name can be found
	if (mMoviesToActorsMap.find(movieName) != mMoviesToActorsMap.end())
	{
		return mMoviesToActorsMap[movieName];
	}
	return sEmptyVector;
}

// Do for Part 2
// Function: findRelationship
// Purpose: Tries to find a relationship between two actors, using a BFS
// and outputs (to cout) the result of the search.
// Input: Name of two actors to check for a relationship
// Returns: Nothing
void IMDBData::findRelationship(const std::string& fromActor, const std::string& toActor)
{

	if ((mActorsToMoviesMap.find(fromActor) != mActorsToMoviesMap.end()) && (mActorsToMoviesMap.find(toActor) != mActorsToMoviesMap.end()))
	{
		bool pathFind = false;
		ActorNode* currentNode = nullptr;
		ActorNode* firstActor = mGraph.getActorNode(fromActor);
		ActorNode* targetActor = mGraph.getActorNode(toActor);

		// 1. Create a queue of ActorNode*, and enqueue the node we¡¯re starting from¡­ 
		std::queue<ActorNode*> myQueue;
		myQueue.push(firstActor);

		// 2. While the GFS queue is not empty¡­ 
		while (!myQueue.empty())
		{

			// a) Dequeue the front Node*, and save in currentNode
			currentNode = myQueue.front();
			myQueue.pop();

			// b) If currentNode == targetNode, we found a path!
			if (currentNode == targetActor)
			{
				pathFind = true;
				
				//print the path
				std::cout << "Found a path!";
				std::cout << " (" << targetActor->mPath.size() << " hops)\n";
				std::cout << firstActor->mName << " was in...\n";

				unsigned i = 0;
				for (auto paths : targetActor->mPath)
				{
					//not the last line of the path
					if (i < targetActor->mPath.size() - 1)
					{
						std::cout << paths.getMovieName() << " with " << paths.getActorName() << " who was in...\n";
					}
					//The last line of the path
					else
					{
						std::cout << paths.getMovieName() << " with " << paths.getActorName() << std::endl;
					}
					//increase the number count of path's line
					i++;
				}
				//break out of the loop
				break;
			}
			else
			{
				// c) If currentNode¡¯s visited bool is false¡­
				if (currentNode->mIsVisited == false)
				{
					// 1. visit currentNode (don't need here)
					// 2. set currentNode's visited bool to true
					currentNode->mIsVisited = true;

					// 3. Loop through currentNode¡¯s adjacency list, and queue all adjacent nodes whose visited flag is false
					for (auto nodes : currentNode->mEdges)
					{

						if (nodes->mOtherActor->mIsVisited == false)
						{
							//a) enqueue the adjacent node
							myQueue.push(nodes->mOtherActor);

							//b) If the adjacent node path has a size of 0¡­ 
							if (nodes->mOtherActor->mPath.size() == 0)
							{

								//a) Set the adjacent node¡¯s path equal to currentNode's path
								nodes->mOtherActor->mPath = currentNode->mPath;

								//b) Push_back adjacent nodes¡¯s name to adjacent node¡¯s path 
								nodes->mOtherActor->mPath.push_back(PathPair(nodes->mMovieName, nodes->mOtherActor->mName));
							}
						}
					}//end of looping through adjacency list
				}//end of if currentNode¡¯s visited bool is false
			}//end of haven't found a path
		}//end of while the queue is not empty

		// 3. If we made it here, we didn't find a path
		if (pathFind == false)
		{
			std::cout << "Didn't find a path :(.\n";
		}
	}
	//if the actor name are invalid
	else
	{
		std::cout << "Invalid actor name!\n";
	}
	// LEAVE THIS AT THE END OF THE FUNCTION
	mGraph.clearVisited();
}

// For Part 2
// Function: createGraph
// Purpose: This helper function constructs the IMDBGraph from the movie to actors map
// Input: None
// Returns: Nothing
void IMDBData::createGraph()
{
	// DO NOT EDIT THIS FUNCTION
	// For every movie in the actors map...
	for (auto& p : mMoviesToActorsMap)
	{
		const std::string& movieName = p.first;
		// Get the actors for this movie
		const std::vector<std::string>& actors = mMoviesToActorsMap[movieName];

		// Make n^2 edges between these actors
		for (size_t i = 0; i < actors.size(); i++)
		{
			ActorNode* firstActor = mGraph.getActorNode(actors[i]);
			for (size_t j = i + 1; j < actors.size(); j++)
			{
				ActorNode* secondActor = mGraph.getActorNode(actors[j]);
				mGraph.createActorEdge(firstActor, secondActor, movieName);
			}
		}
	}
}

// Leave here! Do not edit!!! Use in part 1
// DO NOT REMOVE THIS LINE
std::vector<std::string> IMDBData::sEmptyVector;
