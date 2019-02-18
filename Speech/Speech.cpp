// Speech.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <speechapi_cxx.h>
#include <stdio.h>
#include <fstream>
#include "windows.h" 


using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void recognizeSpeech()
{
	// Creates an instance of a speech config with specified subscription key and service region.
	// Replace with your own subscription key and service region (e.g., "westus").
	auto config = SpeechConfig::FromSubscription("8c5203a67efa4c9a928e2cc92c3cb226", "westus");

	// Creates a speech recognizer.
	auto recognizer = SpeechRecognizer::FromConfig(config);
	cout << "Say something...\n";

	// Performs recognition. RecognizeOnceAsync() returns when the first utterance has been recognized,
	// so it is suitable only for single shot recognition like command or query. For long-running
	// recognition, use StartContinuousRecognitionAsync() instead.
	auto result = recognizer->RecognizeOnceAsync().get();

	// Checks result.


	if (result->Reason == ResultReason::RecognizedSpeech)
	{
		ofstream myfile;
		myfile.open("text.txt", std::ofstream::out | std::ofstream::trunc);
		myfile << result->Text << std::endl;
		myfile.close();

	}
	else if (result->Reason == ResultReason::NoMatch)
	{
		cout << "NOMATCH: Speech could not be recognized." << std::endl;
	}
	else if (result->Reason == ResultReason::Canceled)
	{
		auto cancellation = CancellationDetails::FromResult(result);
		cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

		if (cancellation->Reason == CancellationReason::Error)
		{
			cout << "CANCELED: ErrorCode= " << (int)cancellation->ErrorCode << std::endl;
			cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
			cout << "CANCELED: Did you update the subscription info?" << std::endl;
		}
	}
}

int wmain()
{
	while (1) {
		recognizeSpeech();
		Sleep(1500);
		cout << "END" << endl;
	}
	return 0;
}