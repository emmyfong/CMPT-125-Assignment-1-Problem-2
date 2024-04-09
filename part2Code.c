#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAXFILENAMELENGTH 100
#define MAXFILEOPENATTEMPTS 5
#define MAXERRORMSGLENGTH 100

#define NUM_DIRECTIONS 4
#define NUM_COLOURS 4

enum initTypeList { Random = 1, Checkerboard, AllWhite };
enum colourList { White = 1, Red = 2, Green = 3, Blue = 4 };
enum Direction { West = 1, North = 2, East = 3, South = 4 };

struct Robot {
	int x;
	int y;
	int direction;
	int paintColour;
};

struct GameConfiguration {
	int numRows;
	int numColumns;
	int numRobots;
	int boardType; //Of type initTypeList
	int initSeed;
	int numTurns;
	int intervalTurns;
	char outputFileName[MAXFILENAMELENGTH];
};



// Takes an input string and converts it to an int outValue if possible
// Returns true if possible otherwise false
bool CheckNumericalInput(char* inputString, int* outValue)
{
	// null ptr checks
	if (!inputString || !outValue)
	{
		return false;
	}

	bool bSuccess = true;

	// Check if valid number
	for (int i = 0; i < strlen(inputString); ++i)
	{
		if (!isdigit(inputString[i]))
		{
			bSuccess = false;
			break;
		}
	}

	if (bSuccess)
	{
		*outValue = atoi(inputString);
	}

	return bSuccess;
}


//Return 1 when failed
int ReadIntegerParameterFromFile(FILE* filename, int* outVal, int maxRange, int minRange, const char* varName, char* outErrorMsg)
{
	char inputString[MAXFILENAMELENGTH];
	if (feof(filename))
	{
		sprintf(outErrorMsg, "ERROR: The %s was not in the input file (reached eof)", varName);
		return 1;
	}
	fscanf(filename, "%s", inputString);
	if (!CheckNumericalInput(inputString, outVal))
	{
		sprintf(outErrorMsg, "ERROR: The %s could not be read due to corrupt data in the file", varName);
		return 1;
	}
	if ((*outVal < minRange) || (*outVal > maxRange))
	{
		sprintf(outErrorMsg, "ERROR: The %s was outside the specified range (%d to %d inclusive)", varName, minRange, maxRange);
		return 1;
	}
	return 0;
}

int ReadOutputFile(FILE* filename, char* outVal, char* outErrorMsg)
{
	if (feof(filename))
	{
		sprintf(outErrorMsg, "ERROR: The output file was not in the input file (reached eof)");
		return 1;
	}
	fscanf(filename, "%s", outVal);
	return 0;
}

void outputErrors(const char* errorMsg, FILE* outputFilePointer)
{
	fprintf(stderr, "%s\n", errorMsg);
	if (outputFilePointer)
	{
		fprintf(outputFilePointer, "%s\n", errorMsg);
	}
}

void freeGameBoardMemory(int** gameBoard, int numRows)
{
	if (gameBoard)
	{
		for (int i = 0; i < numRows; i++)
		{
			if (gameBoard[i])
			{
				free(gameBoard[i]);
			}
		}
		free(gameBoard);
	}
}

int ReadInputFile(struct GameConfiguration* configPtr, FILE* outputFile)
{
	const int maxNumRows = 300;
	const int minNumRows = 20;
	const int maxNumColumns = 300;
	const int minNumColumns = 20;
	const int maxNumRobots = 10;
	const int minNumRobots = 1;
	const int maxInitTypeValue = 3;
	const int minInitTypeValue = 1;
	const int maxInitSeed = 32767;
	const int minInitSeed = 10;
	const int maxTurn = 5000;
	const int minTurn = 10;
	const int maxIntervalTurns = maxTurn;
	const int minIntervalTurns = 1;

	int fileOpenAttempts = 0;
	FILE* inputFile = NULL;

	char inputFileName[MAXFILENAMELENGTH];

	while (fileOpenAttempts < MAXFILEOPENATTEMPTS)
	{
		printf("Enter the name of input: ");
		scanf("%s", &inputFileName);
		fileOpenAttempts++;
		inputFile = fopen(inputFileName, "r");

		if (inputFile != NULL)
		{
			break;
		}
		fprintf(stderr, "\nERROR: Input file not opened correctly.\n");
	}

	if (inputFile == NULL)
	{
		printf("ERROR: Failed to open the input file 5 times. Terminating the program.\n");
		return 1;
	}

	// Rows
	char RowErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int RowErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->numRows, maxNumRows, minNumRows, "number of rows", RowErrorMsg);

	// Cols
	char ColumnErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int ColErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->numColumns, maxNumColumns, minNumColumns, "number of columns", ColumnErrorMsg);

	// Robots
	char RobotErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int robotErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->numRobots, maxNumRobots, minNumRobots, "number of robots", RobotErrorMsg);

	// initTypeValue
	char initTypeValueErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int TypeErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->boardType, maxInitTypeValue, minInitTypeValue, "initTypeValue", initTypeValueErrorMsg);

	// initSeed
	char initSeedErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int SeedErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->initSeed, maxInitSeed, minInitSeed, "initSeed", initSeedErrorMsg);

	// turns
	char turnErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int turnErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->numTurns, maxTurn, minTurn, "number of turns", turnErrorMsg);

	// printing interval
	char intervalTurnsErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int intervalTurnsErrorValue = ReadIntegerParameterFromFile(inputFile, &configPtr->intervalTurns, maxIntervalTurns, minIntervalTurns, "number of interval turns", intervalTurnsErrorMsg);

	// Read your output file
	char outputErrorMsg[MAXERRORMSGLENGTH] = { 0 };
	int outputFileNameErrorValue = ReadOutputFile(inputFile, configPtr->outputFileName, outputErrorMsg);

	if (inputFile != NULL)
	{
		fclose(inputFile);
	}

	if (RowErrorValue || ColErrorValue || robotErrorValue || TypeErrorValue || SeedErrorValue || turnErrorValue || turnErrorValue)
	{

		if (outputFileNameErrorValue == 0)
		{
			outputFile = fopen(configPtr->outputFileName, "w");
		}

		// Row
		if (RowErrorMsg)
		{
			outputErrors(RowErrorMsg, outputFile);
		}
		// Col
		if (ColumnErrorMsg)
		{
			outputErrors(ColumnErrorMsg, outputFile);
		}

		// robot
		if (robotErrorValue)
		{
			outputErrors(RobotErrorMsg, outputFile);
		}

		// intial type value
		if (TypeErrorValue)
		{
			outputErrors(initTypeValueErrorMsg, outputFile);
		}

		// initial seed
		if (SeedErrorValue)
		{
			outputErrors(initSeedErrorMsg, outputFile);
		}

		// turns
		if (turnErrorValue)
		{
			outputErrors(turnErrorMsg, outputFile);
		}

		//print interval
		if (intervalTurnsErrorValue)
		{
			outputErrors(intervalTurnsErrorMsg, outputFile);
		}

		if (outputFile != NULL)
		{
			fclose(outputFile);
		}
		return 1;
	}
	return 0;
}

int** AllocateGameBoardArray(int numRows, int numCols, FILE* outputFile)
{
	int** boardpp = (int**)calloc(numRows, sizeof(int*));
	if (boardpp == NULL)
	{
		outputErrors("ERROR: Array of pointers could not be allocated", outputFile);
		return NULL;
	}

	for (int i = 0; i < numRows; i++)
	{
		boardpp[i] = (int*)calloc(numCols, sizeof(int));
		if (boardpp[i] == NULL)
		{
			outputErrors("ERROR: Array storage could not be allocated", outputFile);
			freeGameBoardMemory(boardpp, numRows);
			return NULL;
		}
	}
	return boardpp;
}

void InitBoardRand(int** board, int numRows, int numCols, unsigned int seed)
{
	/* Please note that NUM_COLOURS is a global constant or a MACRO*/
	srand(seed);
	for (int K = 0; K < numRows; K++)
	{
		for (int J = 0; J < numCols; J++)
		{
			board[K][J] = rand() % NUM_COLOURS + 1;
		}
	}
}

void InitBoardChecker(int** board, int numRows, int numCols)
{
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			board[i][j] = ((i + j) % 2 == 0) ? White : Red;	// Even is White, Odd is Red
		}
	}
}

void InitBoardAllWhite(int** board, int numRows, int numCols)
{
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			board[i][j] = White;
		}
	}
}

int InitBoard(int** boardpp, int numRows, int numCols, enum initTypeList myList, unsigned int seed)
{
	switch (myList)
	{
	case Random:
		InitBoardRand(boardpp, numRows, numCols, seed);
		break;
	case Checkerboard:
		InitBoardChecker(boardpp, numRows, numCols);
		break;
	case AllWhite:
		InitBoardAllWhite(boardpp, numRows, numCols);
		break;
	}

	return 0;
}

struct Robot* AllocateRobots(int numRobots, FILE* outputFile)
{
	struct Robot* robots = (struct Robot*)calloc(numRobots, sizeof(struct Robot));
	if (robots == NULL)
	{
		outputErrors("ERROR: Array storage could not be allocated", outputFile);
		return NULL;
	}
	return robots;
}

void InitializeRobots(struct Robot* robots, int numRobots, int numRows, int numCols, unsigned int seed)
{
	srand(seed);

	for (int i = 0; i < numRobots; i++)
	{
		robots[i].x = rand() % numCols;
		robots[i].y = rand() % numRows;

		robots[i].direction = rand() % NUM_DIRECTIONS + 1;
		robots[i].paintColour = rand() % NUM_COLOURS + 1;
        printf("Robot %d(%d, %d) Dir %d Col %d\n", i, robots[i].x, robots[i].y, robots[i].direction, robots[i].paintColour);
	}
}

void MoveRobots(struct Robot* robot, int numRows, int numCols)
{
	int oldX = robot->x;
	int oldY = robot->y;

	switch (robot->direction)
	{
	case North:
		robot->y--;
		if (robot->y < 0)
		{
			robot->y = numRows - 1;
		}
		break;
	case South:
		robot->y++;
		if (robot->y >= numRows)
		{
			robot->y = 0;
		}
		break;
	case East:
		robot->x++;
		if (robot->x >= numCols)
		{
			robot->x = 0;
		}
		break;
	case West:
		robot->x--;
		if (robot->x < 0)
		{
			robot->x = numCols - 1;
		}
		break;
	}
}

void RotateRobot(struct Robot* robot, int color)
{
	int DirectionSequence[] = { North, East, South, West };

	int oldDir = robot->direction;

	int stepSize = 0;
	switch (color)
	{
	case White:
		stepSize = 1;
		break;

	case Red:
		stepSize = 2;
		break;

	case Green:
		stepSize = 3;
		break;

	case Blue:
		stepSize = 0;
		break;
	}

	if (stepSize)
	{
		for (int directionIndex = 0; directionIndex < NUM_DIRECTIONS; directionIndex++)
		{
			if (DirectionSequence[directionIndex] == robot->direction)
			{
				robot->direction = (DirectionSequence[(directionIndex + stepSize) % NUM_DIRECTIONS]);
				break;
			}
		}
	}
}

void PrintBoard(FILE* myStream, int** board, int numRows, int numCols)
{

	for (int K = 0; K < numRows; K++)
	{
		for (int J = 0; J < numCols; J++)
		{
			printf("%d", board[K][J]);
			fprintf(myStream, "%d", board[K][J]);
		}
		printf("\n");
		fprintf(myStream, "\n");
	}
	printf("\n\n");
	fprintf(myStream, "\n\n");
}


int main()
{
	int** boardpp = NULL;
	struct Robot* robots = NULL;
	FILE* outputFile = NULL;
	struct GameConfiguration config;

	if (ReadInputFile(&config, outputFile))
	{
		return 1;
	}

	outputFile = fopen(config.outputFileName, "w");
	boardpp = AllocateGameBoardArray(config.numRows, config.numColumns, outputFile);
	robots = AllocateRobots(config.numRobots, outputFile);
	if ((boardpp == NULL) || (robots == NULL))
	{
		freeGameBoardMemory(boardpp, config.numRows);
		free(robots);
		return 1;
	}

	InitBoard(boardpp, config.numRows, config.numColumns, config.boardType, config.initSeed);
	InitializeRobots(robots, config.numRobots, config.numRows, config.numColumns, config.initSeed);

	for (int turnIndex = 0; turnIndex <= config.numTurns; turnIndex++)
	{
		for (int robotIndex = 0; robotIndex < config.numRobots; robotIndex++)
		{
			if(turnIndex != 0)
			{
				MoveRobots(&robots[robotIndex], config.numRows, config.numColumns);
			}

			// Read color at this tile
			int robotRow = robots[robotIndex].y;
			int robotCol = robots[robotIndex].x;
			int color = boardpp[robotRow][robotCol];

			RotateRobot(&robots[robotIndex], color);

			// Write Robot's color into tile
			boardpp[robots[robotIndex].y][robots[robotIndex].x] = robots[robotIndex].paintColour;
		}
		if (turnIndex % config.intervalTurns == 0)
		{
			PrintBoard(outputFile, boardpp, config.numRows, config.numColumns);
		}
	}

	freeGameBoardMemory(boardpp, config.numRows);
	free(robots);

	return 0;
}

