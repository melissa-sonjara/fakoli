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


class Image extends DataItem
{
	// Fields
	var $fields = array("image_id"				=> Number,
						"title"					=> String,
						"image_file" 			=> String,
						"ALT_tag"				=> String,
						"date_taken"		 	=> Date,
						"credits" 				=> String,
						"caption" 				=> String,
						"keywords" 				=> String,
						"published"				=> Boolean,
						"include_in_slideshow"	=> Boolean,
						"image_type"			=> String,
						"gallery_id"			=> Number);
	
	// Relations
	
	var $relations = array("Gallery"	=>	ImageGallery);
	
	function Gallery()
	{
		return $this->getRelated(ImageGallery);
	}
	
	function Image()
	{
		$this->table = "image";
		$this->primary_key = "image_id";
		
		$this->default_format = "{title}";
		
		$this->DataItem(func_get_args());
	}
	
   function formatThumbnail($width = 75, $height = 0)
	{
		$thumbnail = $this->getThumbnail();
		$text = "<img src='$thumbnail' alt='".htmlSafe($this->title)."' border='0'";
		if($width > 0)	
		{
			$text .= " width='$width'";
		}
		
		if ($height > 0)
		{
			$text .= " height='$height'";
		}
		
		$text .= "/>";
		
		return $text;
	}
	
	function findByFileName($filename)
	{
		$filename = "$filename";
		$images = Query::create(Image, "WHERE image_file=':f'")
					->bind(":f", $filename)
					->execute();
		
		return count($images) > 0 ? $images[0] : null;
	}
	
	function getImage()
	{
		global $config;

		if ($img == "blank" || $img == "")
		{
		 	$img = "/fakoli/images/blank.gif";
		}		 	
		else
		{
			$img = "/action/image/show?image_id={$this->image_id}";
		}
		
		return $img;
	}
	
	function getThumbnail($size = 0)
	{
		$mgr = new ImageManager();
		return $mgr->thumbnailLink($this->image_id, $size);
	}
	
function getCaption()
	{
		return $this->title;
	}
	
	function getShortCredit()
	{
		//this is used to create the correct citation for a library item
		return $this->credits;
	}
}
?>