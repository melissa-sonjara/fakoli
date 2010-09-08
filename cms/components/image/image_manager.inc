<?php

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
	
}