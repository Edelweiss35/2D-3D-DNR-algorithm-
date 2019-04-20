#undef UNICODE

#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef _DEBUG
#pragma comment(lib, "opencv_core400d.lib")
#pragma comment(lib, "opencv_highgui400d.lib")
#pragma comment(lib, "opencv_imgcodecs400d.lib")
#pragma comment(lib, "opencv_imgproc400d.lib")
#pragma comment(lib, "opencv_videoio400d.lib")
#pragma comment(lib, "opencv_photo400d.lib")
#else
#pragma comment(lib, "opencv_core400.lib")
#pragma comment(lib, "opencv_highgui400.lib")
#pragma comment(lib, "opencv_imgcodecs400.lib")
#pragma comment(lib, "opencv_imgproc400.lib")
#pragma comment(lib, "opencv_videoio400.lib")
#pragma comment(lib, "opencv_photo400.lib")
#endif // _DEBUG

using namespace std;

string openVideoFile(const string &title)
{
	OPENFILENAME ofn;
	char filename[MAX_PATH];
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Video Files\0*.mp4;*.avi\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title.c_str();
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		return filename;
	}
	else
	{
		// All this stuff below is to tell you exactly how you messed up above. 
		// Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
		case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
		case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
		case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
		case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
		case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
		case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
		case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
		case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
		case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
		case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
		case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
		case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
		case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
		case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
		default: std::cout << "You cancelled.\n";
		}
	}
	return "";
}

int main()
{
	string fileName = "";
	do {
		fileName = openVideoFile("Select video file");
	} while (fileName.empty());

	cv::VideoCapture video(fileName);
	if (!video.isOpened()) {
		fprintf(stderr, "Error getting camera...\n");
		exit(1);
	}

	cv::Mat orgFrame; // orignal video frame
	cv::Mat gray, frame;
	cv::VideoWriter videoWriter;
	bool bWriterOpen = false;
	int no = 0;
	while (true) {
		video >> orgFrame;
		if (orgFrame.data == nullptr) {
			break;
		}
		cv::fastNlMeansDenoisingColored(orgFrame, frame, 20);
		if (!bWriterOpen) {
			videoWriter.open("result.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, cv::Size(orgFrame.cols, orgFrame.rows), true);
			bWriterOpen = true;
		}
		if (bWriterOpen) {
			no++;
			cout << "Frame No=" << no << endl;
			videoWriter.write(frame);
		}
		if (cv::waitKey(25) == 27) {
			break;
		}
	}
	video.release();
	videoWriter.release();
	cv::destroyAllWindows();
}