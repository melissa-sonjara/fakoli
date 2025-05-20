<?php
/**\addtogroup debug
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

class DebugManager
{
	function __construct()
	{
		
	}
	
	static function setDefaults()
	{
		trace("DebugManager::setDefaults", 3);
		
		global $config;
		
		preg_match_all("/([\\w\\d_]+)\.?([^}]*)/", $config["http_host"], $matches, PREG_SET_ORDER);
		if(count($matches) > 0)
		{
			$matches = $matches[0];
			if(count($matches) > 1)
			{
				$path = $matches[1];
			}
			$tracePath = "/share/". $path . "_trace.log";
		}

		Settings::setDefaultValue("debug", "trace_path", $tracePath, "String", "File path to trace output log", "Trace File", null, 1);
		Settings::setDefaultValue("debug", "trace_level", 3, "Number", "Trace Level for Fakoli core code. Lowest to output only the most serious errors; highest for all", "Trace Levels", "1\n2\n3\n4\n5", 1);
		Settings::setDefaultValue("debug", "trace_detail", "TraceDetailHigh", "String", "The level of detail to be output", "Trace File", "TraceDetailFull\nTraceDetailHigh\nTraceDetailMedium\nTraceDetailNormal", 2);	
		Settings::setDefaultValue("debug", "application_trace_level", 3, "Number", "Trace Level for application code (component pages, handlers, modules).Lowest to output only the most serious errors; highest for all", "Trace Levels", "1\n2\n3\n4\n5", 2);	
		Settings::setDefaultValue("debug", "scheduled_task_trace_level", 3, "Number", "Trace Level for scheduled task workers",  "Trace Levels", "1\n2\n3\n4\n5", 3 );
		Settings::setDefaultValue("debug", "enable_trace_file_downloads", false, Boolean, "Enables remote access for trace files. Leave this disabled for production sites, and only enable if you need to gain access to the trace log but do not have access to SSH", "Trace File", null, 3);
	}
	
}
/**@}*/?>
