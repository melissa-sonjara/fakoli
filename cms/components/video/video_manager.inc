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
 
Fakoli::using("video", "settings");

class VideoManager
{
	function VideoManager()
	{
		trace("Creating VideoManager", 3);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("video", "video_folder", "videos", String, "Directory inside the web folder that is used to store the video files");
		Settings::setDefaultValue("video", "flowplayer_script", "", String, "URI for the Flowplayer script");
		Settings::setDefaultValue("video", "flowplayer_player", "", String, "URI for the Flowplayer SWF file");
		Settings::setDefaultValue("video", "ffmpeg_path", "", String, "Directory containing the ffmpeg binary executables");
	}
	
	static function onInitialize()
	{
		global $html_editor_extra_buttons;
		global $script;
		
		$html_editor_extra_buttons[] =
				array("name"	=>	"video_picker",
					  "image"	=>	"/fakoli/images/video_button.png", 
					  "tooltip"	=>	"Insert Video",
					  "handler"	=>	"videoPicker");
			
		$flowPlayerScript = Settings::getValue("video", "flowplayer_script");
		if ($flowPlayerScript)
		{
			$script .= "<script type='text/javascript' src='$flowPlayerScript'></script>\n";
		}
		
		$flowPlayer = Settings::getValue("video", "flowplayer_player");
		
		$script .= "<script type='text/javascript'>\nvar flowplayerPath = '$flowPlayer';\n</script>\n";
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new VideoUpgradeManager();
		$mgr->upgrade($version);
	}
	
	function hasFFMPEG()
	{
		return (extension_loaded("ffmpeg") && Settings::getValue("video", "ffmpeg_path"));
	}
	
	function open($file)
	{
		trace("Opening video '$file'", 3);
		$this->ffmpeg = new ffmpeg_movie($file);
	}
	
	function getFrameWidth()
	{
		$width = $this->makeMultipleTwo($this->ffmpeg->getFrameWidth());
		trace("Width: $width", 3);
		return $width;	
	}
	
	function getFrameHeight()
	{
		$height = $this->makeMultipleTwo($this->ffmpeg->getFrameHeight());
		trace("Height: $height", 3);
		return $height;	
	}
	
	function getFrameRate()
	{
		return $this->ffmpeg->getFrameRate();
	}
	
	function getNextKeyFrame()
	{
		return $this->ffmpeg->getNextKeyFrame();
	}

	function getFrame($idx)
	{
		return $this->ffmpeg->getFrame($idx);
	}
	
	function makeMultipleTwo($value)
	{
		return intval($value) >> 1 << 1;
	}
	
	/**
	 * Returns the URI that can be used to access the specified thumbnail of a video at the given size.
	 * @param $video_id the ID of the video
	 * @param $size the size of the major axis in pixels
	 * @return string URI that can be used in an <img> tag to view this image.
	 */
	function thumbnailLink($video_id, $size)
	{
		return "/action/video/thumbnail?video_id=$video_id&size=$size";
	}
	
	function imageLink($video_id)
	{
		return "/action/video/show?image_id=$video_id";
	}
	
	/**
	 * Renders the Image specified by the image_id at the specified size.
	 * Note that the size is the length of the major axis, and aspect ratio
	 * is maintained. The generated image is cached after rendering to speed
	 * subsequent accesses. The image file is output with the correct HTTP 
	 * headers to ensure correct interpretation by the browser.
	 * 
	 * @param $image_id the ID of the image to render
	 * @param $size the size of the major axis, in pixels
	 * @param $width the specific width of the rendered thumbnail (if $size is 0)
	 * @param $height the specific height of the rendered thumbnail (if $size and $width are 0)
	 */
	function renderThumbnail($video_id, $size = 0, $width = 0, $height = 0)
	{		
		global $config;
		
		$video = new Video($video_id);

		if (!$size && !$width && !$height) $size = $config['thumbnail_size'];
		
		$suffix = $size ? "$size" : ($width ? "{$width}w" : ($height ? "{$height}h" : ""));
		
		$folder = Settings::getValue("video", "video_folder");
		$dir = $config['homedir']. DIRECTORY_SEPARATOR . $folder;
		
		$imageFile = $dir . DIRECTORY_SEPARATOR . $video->image_file;
		$cacheFile = $dir . DIRECTORY_SEPARATOR . $video->video_id . "_" . $suffix .".png";
		trace("renderThumbnail:: imageFile {$imageFile} and cacheDir {$cacheDir} and cacheFile {$cacheFile}", 3);
		
		if (!file_exists($cacheFile) || (filemtime($cacheFile) < filemtime($imageFile)))
		{
			$src = imagecreatefrompng($imageFile);
 			
			// If the thumbnail hasn't been generated yet, or is out-of-date, create it.
			$fullWidth = imagesx($src);
			$fullHeight = imagesy($src);
			
			if ($size)
			{
				if ($fullWidth > $fullHeight)
				{
					$newWidth = $size;
					$newHeight = intval(($fullHeight * $size) / $fullWidth);
					
				}
				else
				{
					$newWidth = intval(($fullWidth * $size) / $fullHeight);
					$newHeight = $size;
				}
			}
			else if ($width)
			{
				$newWidth = $width;
				$newHeight = intval(($fullHeight * $width) / $fullWidth);
			}
			else if ($height)
			{
				$newHeight = $height;
				$newWidth = intval(($fullWidth * $height) / $fullHeight);
			}
			else
			{
				$newWidth = $fullWidth;
				$newHeight = $fullHeight;
			}
						
			$dst = imagecreatetruecolor($newWidth, $newHeight);
			imagecopyresampled($dst, $src, 0, 0, 0, 0, $newWidth, $newHeight, $fullWidth, $fullHeight);

			if (file_exists($cacheFile))
			{
				// If a previous copy of the file already exists, remove it
				trace("renderThumbnail:: unlinking cachefile", 3);
				unlink($cacheFile);
			}
				
			imagepng($dst, $cacheFile);

			imagedestroy($dst);
			imagedestroy($src);
		}

		$last_modified = filemtime($cacheFile);
		
		$mimeType = getMIMEType($cacheFile);

		header("Last-Modified: ".date('r', $last_modified));
		header("Cache-Control: max-age=300, must-revalidate");
		header("Content-Type: $mimeType");
		
	    if(array_key_exists("HTTP_IF_MODIFIED_SINCE",$_SERVER))
	    {
			trace("renderThumbnail:: array_key_exists(HTTP_IF_MODIFIED_SINCE,_SERVER", 3);
	    	
	        $if_modified_since=strtotime(preg_replace('/;.*$/','',$_SERVER["HTTP_IF_MODIFIED_SINCE"]));
	        
	        if($if_modified_since >= $last_modified)
	        {
				trace("renderThumbnail:: if_modified_since >= last_modified", 3);
	            header("HTTP/1.0 304 Not Modified");
	            exit();
	        }
	    }

	    $f = fopen($cacheFile, "r");

	    $size = filesize($cacheFile);
		header("Content-Length: $size");
		header("Pragma: cache");
		header("Expires:");
		
		fpassthru($f);
		fclose($f);
		trace("renderThumbnail:: exiting", 3);
	}

	
	/**
	 * Event handler to display an image gallery from the specified identifier
	 * @param string $identifier the identifier to match
	 * @param boolean $continue returns whether to continue processing this event
	 */
	static function displayVideoGallery($identifier, &$continue)
	{
		try
		{
			$gallery = Query::create(VideoGallery, "WHERE identifier=:i")
							->bind(":i", $identifier)
							->executeSingle();
							
			$page = ComponentPage::findByIdentifier("video_gallery", "WHERE enabled=1");
			$_GET["gallery_id"] = $gallery->gallery_id;
		
			$pageView = new ComponentPageView($page);
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
				redirect("/login");
			}
			
			echo $pageView->drawView();
			
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
			
		}
		
		return $identifier;
	}
	
	/**
	 * Enumerate the Image Gallery objects.
	 * @param  $items collection that the ImageGallery items are returned in
	 */
	static function enumerateItems($items)
	{
		$galleries = Query::create(VideoGallery, "ORDER BY gallery_name")->execute();
		
		$items["Video Galleries"] = $galleries;
		return $items;
	}
	
		
}
?>