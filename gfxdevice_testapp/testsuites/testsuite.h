#pragma once

#include <string>
#include <wondergui.h>
#include "wg_fileutil.h"

using namespace std;
using namespace wg;

class Test
{
public:
	string name;
	std::function<bool(GfxDevice*, const Rect&)>	init;
	std::function<bool(GfxDevice*, const Rect&)>	run;
	std::function<bool(GfxDevice*, const Rect&)>	exit;
};


class TestSuite 
{
public:
	TestSuite() {}
	virtual ~TestSuite() {}

	virtual bool			init(GfxDevice * pDevice, const Rect& canvas) { return true; }
	virtual bool			exit(GfxDevice * pDevice, const Rect& canvas) { return true; }

	string					name = "Unnamed TestSuite";
	vector<Test>			tests;

protected:

	template<typename T>
	void addTest(string name, bool (T::*initMethod)(GfxDevice * pDevice, const Rect& canvas),
		bool (T::*testMethod)(GfxDevice * pDevice, const Rect& canvas),
		bool (T::*exitMethod)(GfxDevice * pDevice, const Rect& canvas))
	{
		tests.push_back({ name, [this,initMethod](GfxDevice * pDevice, const Rect& canvas) { return (static_cast<T*>(this)->*initMethod)(pDevice,canvas); },
								[this,testMethod](GfxDevice * pDevice, const Rect& canvas) { return (static_cast<T*>(this)->*testMethod)(pDevice,canvas); },
								[this,exitMethod](GfxDevice * pDevice, const Rect& canvas) { return (static_cast<T*>(this)->*exitMethod)(pDevice,canvas); } });
	}

	template<typename T>
	void addTest(string name, bool (T::*testMethod)(GfxDevice * pDevice, const Rect& canvas))
	{
		tests.push_back({ name, nullptr, [this,testMethod](GfxDevice * pDevice, const Rect& canvas) { return (static_cast<T*>(this)->*testMethod)(pDevice,canvas); }, nullptr });
	}
};


