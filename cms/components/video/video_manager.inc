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
	var $movieFile = null;
	
	function VideoManager()
	{
		trace("Creating VideoManager", 3);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("video", "video_folder", "videos", String, "Directory inside the web folder that is used to store the video files", "Video Uploads");
		Settings::setDefaultValue("video", "video_upload_location", "Web Folder", String, 
								  "Indicate whether videos should be stored under the Web Folder or in the document upload area.".
								  "If you do not have mod_xsendfile installed, then Web Folder is strongly recommended", 
								  "Video Uploads", "Web Folder\nDocument Upload Folder\nOther");
		Settings::setDefaultValue("video","video_upload_other_location", "", String, "Full path to alternate video location", "Video Uploads");
		Settings::setDefaultValue("video", "use_HTML5_video", false, Boolean, "Check to enable support for HTML5 video", "Video Player", null, 1);
		Settings::setDefaultValue("video", "use_flash_video", true, Boolean, "Check to enable support for old-style FLV (requires Flowplayer Flash)", "Video Player", null, 2);
		Settings::setDefaultValue("video", "flowplayer_script", "", String, "URI for the Flowplayer script", "Video Player", null, 3);
		Settings::setDefaultValue("video", "flowplayer_player", "", String, "URI for the Flowplayer SWF file", "Video Player", null, 4);
		Settings::setDefaultValue("video", "ffmpeg_path", "", String, "Directory containing the ffmpeg binary executables", "Video Transcoder");
	}
	
	static function onInitialize()
	{
		global $isAction;
		global $isResource;
		
		global $html_editor_extra_buttons;
		global $script;
		global $styles;
		
		$html_editor_extra_buttons[] =
				array("name"	=>	"video_picker",
					  "image"	=>	"/fakoli/images/video_button.png", 
					  "tooltip"	=>	"Insert Video",
					  "handler"	=>	"videoPicker");
			
		$useHTML5 = Settings::getValue("video", "use_HTML5_video");
		$useFlash = Settings::getValue("video", "use_flash_video");
		$flowPlayerScript = Settings::getValue("video", "flowplayer_script");
		
		if ($isAction || $isResource) return;
		
		if ($useHTML5)
		{
			$styles = "<link href='//vjs.zencdn.net/4.12/video-js.css' rel='stylesheet'>" . $styles;
			$script = "<script src='//vjs.zencdn.net/4.12/video.js'></script>" . $script;		
		}
		
		if ($useFlash && $flowPlayerScript)
		{
			$script .= "<script type='text/javascript' src='$flowPlayerScript'></script>\n";
		}
		
		$flowPlayer = Settings::getValue("video", "flowplayer_player");
		
		$script .= "<script type='text/javascript'>\nvar flowplayerPath = '$flowPlayer';\n</script>\n";
	}
	
	static function getVideoFolder()
	{
		global $config;
		
		$location = Settings::getValue("video", "video_upload_location");
		$folder = Settings::getValue("video", "video_folder");
		
		if ($location == "Other")
		{
			$dir = sanitizePath(Settings::getValue("video", "video_upload_other_location") . DIRECTORY_SEPARATOR . $folder);
			trace($dir, 3);
			if (!file_exists($dir))
			{
				//throw new FakoliException("Video Folder does not exist");
			}
		}
		else if ($location == "Web Folder")
		{
			$dir = $config['homedir']. DIRECTORY_SEPARATOR . $folder;
			if (!file_exists($dir))
			{
				throw new FakoliException("Video Folder does not exist");
			}
		}
		else
		{
			$dir = $config['uploadbase']. DIRECTORY_SEPARATOR . $folder;
			if (!file_exists($dir))
			{
				if (!mkdir($dir))
				{
					throw new FakoliException("Could not create video upload folder");
				}
			}
		}
		
		return $dir;
	}
	
	static function getVideoURI($video)
	{
		
		$location = Settings::getValue("video", "video_upload_location");
		
		if ($location == "Web Folder")
		{
			$folder = Settings::getValue("video", "video_folder");			
			$gallery = $video->VideoGallery();
			return "/".$folder."/gallery_".$video->video_gallery_id."/".$video->video_file;
		}
		else
		{
			return "/action/video/stream?video_id=".$video->video_id;
		}
	}	
			
	static function formatVideoURI($video)
	{
		echo VideoManager::getVideoURI($video);
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new VideoUpgradeManager();
		$mgr->upgrade($version);
	}
	
	private function getExecutablePath($exe)
	{
		$path = Settings::getValue("video", "ffmpeg_path");
		$cmd = $path . DIRECTORY_SEPARATOR . $exe;
		if (Fakoli::detectWindowsOS())
		{
			$cmd .= ".exe";
		}
		
		return $cmd;
	}
	
	private function execute($exe, $args)
	{
		if ($exe != "ffmpeg" && $exe != "ffprobe")
		{
			throw new Exception("Invalid FFMPEG executable");
		}
	
		$cmd = $this->getExecutablePath($exe);
		
		if (!file_exists($cmd))
		{
			throw new FakoliException("FFMPEG executable not found");
		}
		
		ob_start();
		passthru("$cmd $args");
		$out = ob_get_contents();
		ob_end_clean();
		return $out;
	}

	function hasFFMPEG()
	{
		$path = Settings::getValue("video", "ffmpeg_path");
		
		if (!$path) 
		{
			trace("FFMPEG: path not set", 3);
			return false;
		}
		
		$ffmpeg = $this->getExecutablePath("ffmpeg");
		if (!file_exists($ffmpeg)) 
		{
			trace("FFMPEG: no executable at $ffmpeg", 3);
			return false;
		}
		
		$ffprobe = $this->getExecutablePath("ffprobe");
		if (!file_exists($ffprobe)) 
		{
			trace("FFMPEG: no executable at $ffprobe", 3);
			return false;
		}
		
		trace("FFMPEG: executables detected in correct location", 3);
		
		return true;
	}
	
	function open($file)
	{
		trace("Opening video '$file'", 3);
		
		if (!file_exists($file))
		{
			throw new FakoliException("File not found");
		}

		$this->movieFile = $file;
		$this->movieInfo = json_decode($this->execute("ffprobe", "-v quiet -print_format json -show_format -show_streams {$this->movieFile}"));
	}
	
	function getFrameWidth()
	{
		foreach($this->movieInfo->streams as $stream)
		{
			if ($stream->codec_type == "video") return $stream->width;
		}
		
		throw new FakoliException("Failed to locate video stream in file");
	}
	
	function getFrameHeight()
	{
		foreach($this->movieInfo->streams as $stream)
		{
			if ($stream->codec_type == "video") return $stream->height;
		}
		
		throw new FakoliException("Failed to locate video stream in file");	
	}
	
	function getFrameRate()
	{
		foreach($this->movieInfo->streams as $stream)
		{
			if ($stream->codec_type == "video") return $stream->r_frame_rate;
		}
		
		throw new FakoliException("Failed to locate video stream in file");	
	}

	function getFrame($idx)
	{
		return $this->ffmpeg->getFrame($idx);
	}
	
	function saveFrame($offset, $out)
	{
		if (!$this->movieFile) throw new FakoliException("Movie File not loaded");
		$width = $this->getFrameWidth();
		$height = $this->getFrameHeight();
		
		$this->execute("ffmpeg", "-ss {$offset} -i {$this->movieFile} -t 1 -s {$width}x{$height} -f image2 {$out}");
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

		//AJG - default to natural size if not specified
		if (!$size && !$width && !$height) 
		{
			$width = $video->width;
			$height = $video->height;
		}
		
		$suffix = $size ? "$size" : ($width ? "{$width}w" : ($height ? "{$height}h" : ""));
		
		$dir = $video->VideoGallery()->getGalleryDirectory();
		
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
			
			trace("Rendering $cacheFile @ $newWidth x $newHeight", 3);
			
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

		Fakoli::sendFile($cacheFile);

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
			$_GET["gallery_id"] = $gallery->video_gallery_id;
		
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
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component video is deleting objects dependent on user_id {$user_id}", 3);
	
		$tx = new DataTransaction();
		
		try
		{
			$galleries = Query::create(VideoGallery, "WHERE owner_id=:owner_id")
				->bind(":owner_id", $user_id)
				->execute();
			
			if(count($galleries))
			{
				foreach($galleries as $gallery)
				{
					$gallery->joinTransaction($tx);
					VideoManager::deleteGallery($gallery);
				}
			}
			
			$video = new Video();
			$video->joinTransaction($tx);
			$video->delete("WHERE user_id={$user_id}");
			
			$download = new VideoDownload();
			$download->joinTransaction($tx);
			$download->delete("WHERE user_id={$user_id}");
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
		
		return $user;
	}
	
	/**
	 * When deleting a video gallery, we must also delete all videos linked to the
	 * gallery and any downloads of those videos.
	 * 
	 * @param obj $gallery class VideoGallery
	 */
	static function deleteGallery($gallery)
	{
		$tx = new DataTransaction();
		
		try
		{
			$videos = $gallery->Videos();
			
			if(count($videos))
			{
				$download = new VideoDownload();
				foreach($videos as $video)
				{
					$download->joinTransaction($tx);
					$video->joinTransaction($tx);
					$download->delete("WHERE video_id={$video->video_id}");
					$video->delete();
				}
			}
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}		
	}
	
	static function videoGalleryTabs($key)
	{
		$tabs = array(	
				"Gallery" 		=>	"/admin/video_gallery_form",
				"Permissions"	=>	"/admin/video_gallery_permissions",					
				"Videos" 		=>	"/admin/videos",
				"Statistics"	=>	"/admin/video_gallery_stats",
				);

		$qs = ($key) ? "video_gallery_id=$key" : ""; 
		return new TabBar("tabs", $tabs, $qs);
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

	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Video;
		return $classes;
	}
		
}
?>