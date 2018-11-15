#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <locale>

// A helper class to measure CPU performance. 
// To measure GPU performance use OpenGLProfiler.
class CpuProfiler
{
	typedef std::chrono::high_resolution_clock clk;
// variables 
private:
	std::vector<clk::time_point> timeStamps;
	std::vector<clk::duration> durations;
	std::ofstream file;
	std::stringstream stream;
	bool createOutput = false;			// whether or not to generate a formatted string.
	bool outputMilliseconds = false;	// output milliseconds or nanoseconds.
	bool printingOutput = false;		// print formatted string to console.
	bool printingOutputFlushed = false;	// print console on new-line or current-line.
	bool loggingOutput = false;			// log formatted string to file.
	unsigned int frame;

// functions 
public:
	CpuProfiler() 
	{
		stream.imbue(std::locale(""));
		stream << std::setprecision(2) << std::fixed;
	}
	~CpuProfiler()
	{
		file.close();
	}

	// Creates count amount of timers to measure CPU commands.
	// Calling this function clears any previous timers. 
	void InitializeTimers(unsigned int count)
	{
		timeStamps.resize(count);
		durations.resize(count);
	}

	// Start measuring duration for a specified timer. Timer's are 0 based. 
	void StartTimer(unsigned int timer)
	{
		timeStamps[timer] = clk::now();
	}

	// Stop measuring duration for a specified timer. Timer's are 0 based.
	void StopTimer(unsigned int timer)
	{
		durations[timer] = clk::now() - timeStamps[timer];
	}

	// Gets the duration for a timer in nanoseconds. Timer's are 0 based.
	long long GetDuration(unsigned int timer)
	{
		return durations[timer].count();
	}

	// Gets the duration in milliseconds. Timer's are 0 based.
	long long GetDurationMs(unsigned int timer)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(durations[timer]).count();
	}

	// Gets the duration as a fraction of a second. Timer's are 0 based.
	double GetDurationSec(unsigned int timer)
	{
		return std::chrono::duration<double>(durations[timer]).count();
	}

	// Finish measuring time for this frame. Only call this if you want auto-generated output.
	void FrameFinish()
	{
		if (createOutput)
		{
			stream.str("");
			stream << ++frame << ":";
			for (int i = 0; i < durations.size(); i++)
			{
				if (outputMilliseconds)
					stream << "   [" << i << "]: " << GetDurationMs(i);
				else
					stream << "   [" << i << "]: " << GetDuration(i);
			}
			if (printingOutput)
			{
				if (printingOutputFlushed)
					std::cout << "\r" << stream.str() << std::flush;
				else 
					std::cout << stream.str() << std::endl;
			}
			if (loggingOutput)
			{
				file << stream.str() << std::endl;
			}
		}
	}

#pragma region Logging functions 

	// should formatted string be logged in a file
	void LogOutput(const std::string filename)
	{
		file.open("logs/" + filename, std::ofstream::out | std::ofstream::trunc);
		loggingOutput = true;
		createOutput = true;
	}

	// should formatted string be printed on console
	void PrintOutput(bool singleLine)
	{
		std::cout << std::setw(2) << std::setfill('0');
		printingOutput = true;
		printingOutputFlushed = singleLine;
		createOutput = true;
	}

	// should formatted string show duration in milliseconds or nanoseconds?
	void FormatMilliseconds(bool torf)
	{
		outputMilliseconds = torf;
	}

	// get formatted string
	std::string GetOutput()
	{
		return stream.str();
	}

#pragma endregion
};

