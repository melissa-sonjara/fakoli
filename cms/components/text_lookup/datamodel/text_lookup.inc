<?php
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

Fakoli::using("email");

class TextLookup extends DataItem
{
	var $fields = array(
			"text_id"	=>	Number,
			"code"		=>	String,
			"text"		=>	HTML, // default language American English
			"class_name"	=>	String
	);
	

	var $relations = array(
			"Translations"	=>	TextTranslation	
	);
	
	
	function Translations($constraint = "")
	{
		return $this->getRelatedList(TextTranslation, "", $constraint);
	}
	
	/*
	 * Retrieves text for display on a page, given the
	 * code. If an object is passed, then attempt to
	 * retrieve any merge codes values for codes used in the 
	 * text that have been saved to merge_code table under 
	 * the same class name as the $obj.
	 * 
	 * $code - the value of the code field of the text object
	 * 
	 * $obj - a data item instance that can retrieve values
	 * for merge codes used in the block of text (the text
	 * field value of table text_lookup.
	 */
	static function getText($code, $obj = null)
	{
		$texts = Query::create(TextLookup, "WHERE code=:c")
			->bind(":c", $code)
			->execute();
			
		if(count($texts) > 0)
		{
			$text = $texts[0];
			if(isset($obj))
			{
				$mgr = new MergeCodeManager($obj, $text->text);
				return $mgr->searchAndReplace($text->text);
			}
			else
				return $text->text;
		}
		else // return the code to signal that no value was found.
			return $code;
	}
	

	function TextLookup()
	{
		$this->primary_key = "text_id";
		$this->table = "text_lookup";

		$this->DataItem(func_get_args());
	}
}


class TextTranslation extends DataItem
{
	var $fields = array(
			"text_translation_id"	=>	Number,
			"text_id"				=>	String,
			"language"				=>	String,
			"text"					=>	HTML
	);
	
	var $relations = array(
			"TextLookup"			=>	TextLookup
	);
	
	static $languageOptions = array(
		"French"	=>	"French",
		"Spanish"	=>	"Spanish",
		"German"	=>	"German",
		"Russian"	=>	"Russian"
	);
	
	function TextLookup()
	{
		return $this->getRelated(TextLookup);
	}
	
	function TextTranslation()
	{
		$this->primary_key = "text_translation_id";
		$this->table = "text_translation";

		$this->DataItem(func_get_args());
	}	
}