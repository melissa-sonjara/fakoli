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
 
class ImageManager
{
	function ImageManager()
	{
	}
	
	function renderThumbnail($image_id, $size = 0)
	{		
		global $config;
		
		$image = new Image($image_id);
		$gallery = $image->Gallery();

		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}
		
		if (!$size) $size = $config['thumbnail_size'];
		
		$ext = strtolower(substr($image->image_file, -4));

		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		$cacheDir = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . "thumbnail-cache";
		$cacheFile = $cacheDir . DIRECTORY_SEPARATOR . $image->image_id . "_" . $size . $ext;
		
		if (!file_exists($cacheFile) || (filemtime($cacheFile) < filemtime($imageFile)))
		{
		 	switch($ext)
 			{
 			case ".jpg":
 				$src = imagecreatefromjpeg($imageFile);
 				break;
 			
 			case ".png":
 				$src = imagecreatefrompng($imageFile);
 				break;
 			}
 			
			// If the thumbnail hasn't been generated yet, or is out-of-date, create it.
			$fullWidth = imagesx($src);
			$fullHeight = imagesy($src);
			
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
			
			$dst = imagecreatetruecolor($newWidth, $newHeight);
			imagecopyresampled($dst, $src, 0, 0, 0, 0, $newWidth, $newHeight, $fullWidth, $fullHeight);
			
			if (!file_exists($cacheDir))
			{
				mkdir($cacheDir);
			}
			
			if (file_exists($cacheFile))
			{
				// If a previous copy of the file already exists, remove it
				unlink($cacheFile);
			}
			
			switch($ext)
			{
			case ".jpg":
				
				imagejpeg($dst, $cacheFile, 85);
				break;
				
			case ".png":
				
				imagepng($dst, $cacheFile);
				break;
			}

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
	    	
	        $if_modified_since=strtotime(preg_replace('/;.*$/','',$_SERVER["HTTP_IF_MODIFIED_SINCE"]));
	        
	        if($if_modified_since >= $last_modified)
	        {
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
		
	}
	
	function renderImage($image_id)
	{
		$image = new Image($image_id);
		$gallery = $image->Gallery();
		
		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}

		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		
		$last_modified = filemtime($imageFile);
		
		$mimeType = getMIMEType($imageFile);

		header("Last-Modified: ".date('r', $last_modified));
		header("Cache-Control: max-age=300, must-revalidate");
		header("Content-Type: $mimeType");
		
	    if(array_key_exists("HTTP_IF_MODIFIED_SINCE",$_SERVER))
	    {
	    	
	        $if_modified_since=strtotime(preg_replace('/;.*$/','',$_SERVER["HTTP_IF_MODIFIED_SINCE"]));
	        
	        if($if_modified_since >= $last_modified)
	        {
	            header("HTTP/1.0 304 Not Modified");
	            exit();
	        }
	    }

	    $f = fopen($imageFile, "r");

	    $size = filesize($imageFile);
		header("Content-Length: $size");
		header("Pragma: cache");
		header("Expires:");
		
		fpassthru($f);
		fclose($f);
		
	}
		
	function thumbnailLink($image_id, $size)
	{
		return "/action/image/thumbnail?image_id=$image_id&size=$size";
	}
	
	function getSize($image_id, &$width, &$height)
	{
		$image = new Image($image_id);
		$gallery = $image->Gallery();
		
		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}

		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		
		$img = imageCreateFromString(file_get_contents($imageFile));
		$width = imagesx($img);
		$height = imagesy($img);
		imageDestroy($img);
	}

	function getScaledSize($image_id, $size, &$width, &$height)
	{
		$this->getSize($image_id, $width, $height);
		
		if ($width > $height)
		{
			$height = $height * $size / $width;
			$width = $size;
		}
		else
		{
			$width = $width * $size / $height;
			$height = $size;
		}
	}
	
	static function displayImageGallery($identifier, &$continue)
	{
		try
		{
			$gallery = Query::create(ImageGallery, "WHERE identifier=:i")
							->bind(":i", $identifier)
							->executeSingle();
							
			$page = ComponentPage::findByIdentifier("image_gallery", "WHERE enabled=1");
			$_GET["gallery_id"] = $gallery->gallery_id;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
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
	
	static function enumerateItems(&$items)
	{
		$galleries = Query::create(ImageGallery, "ORDER BY title")->execute();
		
		$items["Image Galleries"] = $galleries;
		return $items;
	}
}