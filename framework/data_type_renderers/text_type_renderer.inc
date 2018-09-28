<?php

/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

require_once realpath(dirname(__FILE__)."/abstract_type_renderer.inc");

class TextTypeRenderer extends AbstractTypeRenderer
{	
	function TextTypeRenderer()
	{
	}
	
	static function format($value, $template = "")
	{			
		if (!$template) return $value;
		
	
		if ($template[0] == '/')
		{
			$clauses = array();
			if (preg_match("/(\\/.*?\\/)(.*?)\\//", $template, $clauses))
			{
				return preg_replace($clauses[1], $clauses[2], $value);
			}
		}

		if ($template[0] == '|')
		{
			$clauses = array();
			if (preg_match("/(|.*?|)(.*?)|/", $template, $clauses))
			{
				return preg_replace($clauses[1], $clauses[2], $value);
			}
		}

		switch($template)
		{
		case "codify":
			return codify($value);

		case "prettify":
			return prettify($value);
		
		case "upper":
			return strtoupper($value);
			
		case "lower":
			return strtolower($value);
			
		case "ucfirst":
			return ucfirst($value);
			
		case "lcfirst":
			return lcfirst($value);
			
		case "ucwords":
			return ucwords(strtolower($value));
			
		case "stripHTML":
			return stripHTML($value);
			
		case "jsSafe":
			return jsSafe($value);
		
		case "html":
			return formatAsHTML($value);
			
		case "htmlSafe":
			return htmlsafe($value);
			
		case "HTML":
			return formatAsHtml($value);

		case "xml":
			return xmlEntities($value);
			
		case "xmlSafe":
			return xmlEntities(stripHTML($value));
			
		case "firstSentence":
			return firstSentence($value);
			
		default:
			
			if (is_numeric($template))
			{
				return ellipsis(stripHTML($value), $template, true);
			}
		}
		
		return $value; 
	}
}
