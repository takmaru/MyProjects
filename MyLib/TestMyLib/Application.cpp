#include "stdafx.h"
#include "CppUnitTest.h"

#include "../MyLib/Application/ApplicationInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMyLib {
	TEST_CLASS(Application) {
	public:
		TEST_METHOD(TestApplicationInfo) {
			Assert::AreEqual(MyLib::App::modulefilename().c_str(),
				L"C:\\PROGRAM FILES (X86)\\MICROSOFT VISUAL STUDIO 11.0\\COMMON7\\IDE\\COMMONEXTENSIONS\\MICROSOFT\\TESTWINDOW\\vstest.executionengine.x86.exe");
			Assert::AreEqual(MyLib::App::modulepath().c_str(),
				L"C:\\PROGRAM FILES (X86)\\MICROSOFT VISUAL STUDIO 11.0\\COMMON7\\IDE\\COMMONEXTENSIONS\\MICROSOFT\\TESTWINDOW");
		}
	};
}
