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

class EmbeddedSlideshowHandler
{
	static function postProcessContent($content)
	{
		global $script;
		
		$matches = array();
		
		if (preg_match("/\\{slideshow:(?:<span>)?([\\w\\d_]+)(?:<\\/span>)?:(\\d+)\\s*x\\s*(\\d+)\\}/i", $content, $matches))
		{
			$gallery_id = $matches[1];
			$width = $matches[2];
			$height = $matches[3];
			
			if (is_numeric($gallery_id))
			{
				$gallery = new ImageGallery($gallery_id);
			}
			else
			{
				$gallery = ImageGallery::findByIdentifier($gallery_id);
			}
			
			$images = $gallery->Images("WHERE include_in_slideshow=1 ORDER BY sort_order, image_id");
			
			$slideshow = new Slideshow($images, $width, $height);
			$slideshow->preload = false;
			
			$script .= $slideshow->writeScript();
			
			ob_start();
			
			$slideshow->writeHTML();
			
			$sout = ob_get_contents();
			
			ob_end_clean();
			
			$content = str_replace($matches[0], $sout, $content);
		}
		
		return $content;
	}
}


?>