<?php
/**\addtogroup text_lookup
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


Fakoli::using("text_lookup");

class TextLookupSerializationHandler
{
	function __construct()
	{
	}


	function export()
	{
		$xml = "\n<TextMap>";
		$xml .= SerializationManager::serialize(TextLookup, "ORDER BY text_id");
		$xml .= SerializationManager::serialize(TextTranslation, "ORDER BY text_translation_id");
		$xml .= "\n</TextMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(TextLookup, $doc, $tx);
		SerializationManager::unserialize(TextTranslation, $doc, $tx);
	}		
}

class TextLookupManager
{
	
	function __construct()
	{
	}

	static function deleteText($text)
	{
		$translations = $text->Translations();
		
		if(count($translations) > 0)
		{
			foreach($translations as $translation)
				$translation->delete();
		}
		
		$text->delete();
		
	}
	
	static function getTabs($key = "")
	{
		$tabs = array(
			"Text Lookup"			=>	"/admin/text_lookup_form",
			"Translations"		=>	"/admin/text_translations"
			);

		$qs = ($key) ? "text_id=$key" : ""; 
		$tabs = new TabBar("tabs", $tabs, $qs);
		return $tabs;
	}
	

	static function setDefaults()
	{
		$options = implode("\n", array_keys(TextTranslation::$languageOptions));
		Settings::setDefaultValue("text_lookup", "language", "English", "String", "", "", $options);
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new TextLookupUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("text_lookup", "Text Lookups and Translations", new TextLookupSerializationHandler());
		return true;
	}		
	
	static function registerVersionedContent()
	{
		VersionedContentManager::registerVersionedContent('TextLookup');
	}
	
}
/**@}*/?>
