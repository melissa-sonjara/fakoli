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




class TextLookupManager
{
	var $item; // dataitem object of the base class sending the emails
	var $lookup;
	
	function TextLookupManager($item, $lookup)
	{
		$this->item = clone($item);
		$this->item->filter = null;
		$this->lookup = $lookup;
	}


	/*
	 * Get all the merge codes used in the recipient, subject, and
	 * message fields in the template.
	 * 
	 * Given the merge codes in use in the template, retrieve the
	 * maps for those merge codes in the database.
	 * 
	 * For each used merge code, if a record exists in merge
	 * code table, then use that map; if not, treat the
	 * code itself as a map. This allows fields in the
	 * sending class to be accessed without a merge code record.
	 * E.g., merge code "first_name" access the first_name field
	 * in the sending class.
	 *  
	 */
	function merge()
	{
		$usedMergeCodes = array(); 
		
		preg_match_all("/\[(.*?)\]/", $this->lookup->text, $usedMergeCodes, PREG_PATTERN_ORDER);
	
		$usedMergeCodes = $usedMergeCodes[1];		

		if(count($usedMergeCodes) > 0)
		{
			$used = strip_tags("'". implode("','", array_values($usedMergeCodes)) . "'");
		
			$mergeCodes = Query::create(MergeCode, "WHERE class_name = :cn and name in ($used)")
				->bind(":cn", $this->lookup->class_name)
				->execute();
		}
		
		if(count($mergeCodes) > 0)
			$indexed = reindexList($mergeCodes, "name");
		else
			$indexed = array();
	
		if(count($usedMergeCodes) > 0)
		{
			$patterns = array();
			$replacements = array();
				
			foreach($usedMergeCodes as $idx => $name)
			{
				$clean_name = strip_tags($name);
				if(array_key_exists($clean_name, $indexed))
					$mergeCode = $indexed[$clean_name];
				else
				{
					$mergeCode = new MergeCode();
					$mergeCode->map = $clean_name;
				} 
				
				$map = ($mergeCode) ? $mergeCode->map : $clean_name;
				
				trace("EmailManager:: map is $map", 3);
					
				if($map)
					$replaceText = $this->getReplaceText($map);

				/*
				 *  Even if replace text is empty, still add to
				 *  list to get rid of code from the text (e.g., [merge_code]).
				 *  Search on the "dirty" name that could contain stray
				 *  html so that the text is replaced.
				 */
		
				array_push($patterns, "[$name]");
				array_push($replacements, $replaceText);
			}
		}

		$this->lookup->text = str_replace($patterns, $replacements, $this->lookup->text);			
	}
	
	
	/*
	 * If the mergeCode map has 2 parts, then possible interpretations: 
	 * a) Part 1 is relation and part 2 is field of relation
	 * b) part 1 is relation and part 2 is function in that relation
	 * c) part 1 is a class and part2 is a static function in that class
	 *
	 * 
	 * If the mergeCode map has 1 part, then it must be
	 * a) callback function, no class
	 * b) function of calling class
	 * c) field of calling class
	 * 
	 * Check if has field before check if is_callable in case the field
	 * name could be interpreted as callable (e.g., "date)
	 */
	function getReplaceText($mergeCodeMap)
	{	
		$valid = true;
					
		list($part1, $part2) = explode(".", $mergeCodeMap);

		trace("EmailManager::part1 is $part1 and part2 is $part2", 3);
			
		if(get_class($this->item) == $part1)
		{
			trace("EmailManager::part1 equals item class", 3);
			$part1 = $part2;
			$part2 = "";
		}			
				
		if($part2)
		{
			if($this->item->hasRelation($part1))
			{
				$replaceObj = $this->item->$part1();
				if(is_object($replaceObj) && $replaceObj->hasField($part2))
				{
					trace("EmailManager:: part1 is relation and part2 is a field in that related class.", 3);
					$replaceText = $replaceObj->format("{".$part2 ."}");
				}
				elseif(is_callable(array($replaceObj, $part2)))
				{
					trace("EmailManager:: part1 is relation of sending class and part2 is a function in that relation.", 3);
					$replaceText = $replaceObj->$part2();
				}	
				else
				{
					$valid = false;
				}
			}
			elseif(is_callable(array($part1, $part2)))
			{
				trace("EmailManager:: part1 is class and part2 is static function of a class.", 3);
				$replaceText = call_user_func(array($part1, $part2), $this->item);
			}
			else
			{
				$valid = false;
			}
		}
		elseif($part1) // Map has just one part
		{
			if($this->item->hasField($part1))
			{
				trace("EmailManager:: part1 is field of sending class.", 3);
				$replaceText = $this->item->format("{".$part1 ."}");
			}
			elseif(is_callable($part1))
			{
				trace("EmailManager:: part1 is callback function.", 3);
				$replaceText = call_user_func($part1, $this->item);
			}
			elseif(is_callable(array($this->item, $part1)))
			{
				trace("EmailManager:: part1 is function from sending class.", 3);
				$replaceText = $this->item->$part1();
			}
			else
			{
				$valid = false;
			}
		}
		
		if($valid)
			trace("EmailManager:: replaceText is $replaceText", 3);
		else
			trace("EmailManager:: Warning: merge code map not valid.", 3);
	
		return $replaceText;		
	}
	

	function getRecipients()
	{
		$validRecipients = array();
		
		trace("EmailManager:getRecipients {$this->recipients}", 3);
		if($this->recipients)
		{
			$recipients = explode(",", $this->recipients);
			if(count($recipients) > 0)
			{
				foreach($recipients as $recipient)
				{
					if(preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
						$validRecipients[] = $recipient;
					else
						trace("EmailManager:getRecipients invalid recipient $recipient", 3);
						
				}
			}
		}

		return $validRecipients;
	}
	
	/*
	 * Call the callback whether successful or not; calling script
	 * decides how to handle either way. Each recipient could have
	 * different result so we don't record multiple recipients in
	 * one record.
	 */
	
	function sendEmail()
	{
		$recipients = $this->getRecipients();
		$rtn = false;
		
		if(count($recipients) > 0)
		{
			foreach($recipients as $recipient)
			{
				$rtn = $this->sendOneEmail($recipient);
			}
		}
		else
		{
			trace("EmailManager:: Warning - no valid recipients", 3);
		}
		return $rtn;
	}
	

}