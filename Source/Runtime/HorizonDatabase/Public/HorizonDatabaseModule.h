/******************************************************
* Boost Software License - Version 1.0 - 2016/10/06
*
* Copyright (c) 2016 dorgon chang
* http://dorgon.horizon-studio.net/
*
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
*
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
**********************************************************/

#pragma once

// Created by dorgon, All Rights Reserved.
// email: dorgonman@hotmail.com
// blog: dorgon.horizon-studio.net

#include "ModuleManager.h"




class IHorizonDatabaseModule
	: public IModuleInterface
{
public:
	/**
	*  Singleton-like access to this module's interface. This is just for convenience!
	*  Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
	*
	*  @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline IHorizonDatabaseModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IHorizonDatabaseModule >("HorizonDatabaseModule");
	}

	/**
	*  Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	*
	*  @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("HorizonDatabaseModule");
	}

	
};