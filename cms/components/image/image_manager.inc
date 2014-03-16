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

class ImageSerializationHandler
{
	function ImageSerializationHandler()
	{
	}


	function export()
	{
		$xml  = "\n<ImageMap>";
		$xml .= SerializationManager::serialize(ImageGallery, "ORDER BY gallery_id");
		$xml .= SerializationManager::serialize(ImageRecord, "ORDER BY image_id");
		$xml .= "</ImageMap>";

		return $xml;
	}

	function import($doc, $tx)
	{
		SerializationManager::unserialize(ImageGallery, $doc, $tx);
		SerializationManager::unserialize(ImageRecord, $doc, $tx);
	}
}

/**
 * The ImageManager class provides basic Image manipulation functions
 * that are useful when working with Image objects.
 * @author andy
 *
 */
class ImageManager
{
	/**
	 * Creates a new ImageManager
	 */
	function ImageManager()
	{
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
	function renderThumbnail($image_id, $size = 0, $width = 0, $height = 0)
	{		
		global $config;
		
		$image = new ImageRecord($image_id);
		$gallery = $image->Gallery();

		trace("renderThumbnail:: image = {$image->image_id} and gallery = {$gallery->gallery_id}", 3);
		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}
		
		if (!$size && !$width && !$height) $size = $config['thumbnail_size'];
		
		$suffix = $size ? "$size" : ($width ? "{$width}w" : ($height ? "{$height}h" : ""));
		
		$nameParts = explode(".",$image->image_file);
		$ext = ".".strtolower($nameParts[1]);

		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		$cacheDir = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . "thumbnail-cache";
		$cacheFile = $cacheDir . DIRECTORY_SEPARATOR . $image->image_id . "_" . $suffix . $ext;
		trace("renderThumbnail:: imageFile {$imageFile} and cacheDir {$cacheDir} and cacheFile {$cacheFile}", 3);
		
		if (!file_exists($cacheFile) || (filemtime($cacheFile) < filemtime($imageFile)))
		{
		 	switch($ext)
 			{
 			case ".jpg":
 				$src = imagecreatefromjpeg($imageFile);
 				break;

 			case ".jpeg":
 				$src = imagecreatefromjpeg($imageFile);
 				break;
				
 			case ".png":
 				$src = imagecreatefrompng($imageFile);
 				break;
 			}
 			
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
			imagealphablending($dst, false);
			imagesavealpha($dst, true);
				
			$trans = imagecolorallocatealpha($dst, 255, 255, 255, 127);
			$white = imagecolorallocate($dst, 255, 255, 255);
				
			imagecolortransparent($dst, $trans);
			
			imagefilledrectangle($dst, 0, 0, $newWidth, $newHeight, $white);
			imagefilledrectangle($dst, 0, 0, $newWidth, $newHeight, $trans);
			
			imagecopyresampled($dst, $src, 0, 0, 0, 0, $newWidth, $newHeight, $fullWidth, $fullHeight);
			
			if (!file_exists($cacheDir))
			{
				trace("renderThumbnail:: creating cachedir", 3);
				mkdir($cacheDir);
			}
			
			if (file_exists($cacheFile))
			{
				// If a previous copy of the file already exists, remove it
				trace("renderThumbnail:: unlinking cachefile", 3);
				unlink($cacheFile);
			}
			
			switch($ext)
			{
			case ".jpg":
				
				imagejpeg($dst, $cacheFile, 85);
				break;
				
			case ".jpeg":
				
				imagejpeg($dst, $cacheFile, 85);
				break;

			case ".png":
				
				imagepng($dst, $cacheFile);
				break;
			}

			imagedestroy($dst);
			imagedestroy($src);
		}

		Fakoli::sendFile($cacheFile);
	}
	
	/**
	 * Renders the specified image at its natural size. The image file is output with the correct HTTP 
	 * headers to ensure correct interpretation by the browser.
	 * @param $image_id the ID of the image to be rendered.
	 */
	function renderImage($image_id)
	{
		$image = new ImageRecord($image_id);
		$gallery = $image->Gallery();
		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		
		Fakoli::sendFile($imageFile);
	}
		
	function resizeAndCropToFit($image_id, $width, $height)
	{
		global $config;
		
		$image = new ImageRecord($image_id);
		$gallery = $image->Gallery();
		
		trace("resizeAndCropToFit:: image = {$image->image_id} and gallery = {$gallery->gallery_id}", 3);
		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}
		
		if (!$width || !$height) $size = $config['thumbnail_size'];
		
		$suffix = "cropTo_{$width}x{$height}";
		
		$nameParts = explode(".",$image->image_file);
		$ext = ".".strtolower($nameParts[1]);
		
		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		$cacheDir = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . "thumbnail-cache";
		$cacheFile = $cacheDir . DIRECTORY_SEPARATOR . $image->image_id . "_" . $suffix . ".png";
		trace("renderThumbnail:: imageFile {$imageFile} and cacheDir {$cacheDir} and cacheFile {$cacheFile}", 3);
		
		if (!file_exists($cacheFile) || (filemtime($cacheFile) < filemtime($imageFile)))
		{
			switch($ext)
			{
				case ".jpg":
					$src = imagecreatefromjpeg($imageFile);
					break;
		
				case ".jpeg":
					$src = imagecreatefromjpeg($imageFile);
					break;
		
				case ".png":
					$src = imagecreatefrompng($imageFile);
					break;
			}
		
			// If the thumbnail hasn't been generated yet, or is out-of-date, create it.
			$fullWidth = imagesx($src);
			$fullHeight = imagesy($src);

			$aspect = $width / $height;
			
			$srcHeight = $fullWidth / $aspect;
			$offset = ($fullHeight - $srcHeight) / 2;
			
			$dst = imagecreatetruecolor($width, $height);
			imagealphablending($dst, false);
			imagesavealpha($dst, true);
			
			$trans = imagecolorallocatealpha($dst, 255, 255, 255, 127);
			imagecolortransparent($dst, $trans);
			
			imagefilledrectangle($dst, 0, 0, $width, $height, $trans);
			
			imagecopyresampled($dst, $src, 0, 0, 0, $offset, $width, $height, $fullWidth, $srcHeight);
				
			if (!file_exists($cacheDir))
			{
				trace("renderThumbnail:: creating cachedir", 3);
				mkdir($cacheDir);
			}
				
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
	}
	

	function iconize($image_id, $size)
	{
		global $config;
	
		$image = new ImageRecord($image_id);
		$gallery = $image->Gallery();
	
		trace("iconize:: image = {$image->image_id} and gallery = {$gallery->gallery_id}", 3);
		//TODO: Fix this
		//if (!checkRole($gallery->read_access))
		//{
		//	throw new FakoliException("Access Denied");
		//}
	
		if (!$size) $size = $config['thumbnail_size'];
	
		$suffix = "icon_$size";
	
			$nameParts = explode(".",$image->image_file);
		$ext = ".".strtolower($nameParts[1]);

		$imageFile = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . $image->image_file;
		$cacheDir = $gallery->getGalleryDirectory() . DIRECTORY_SEPARATOR . "thumbnail-cache";
		$cacheFile = $cacheDir . DIRECTORY_SEPARATOR . $image->image_id . "_" . $suffix . ".png";
		trace("renderThumbnail:: imageFile {$imageFile} and cacheDir {$cacheDir} and cacheFile {$cacheFile}", 3);
		
		if (!file_exists($cacheFile) || (filemtime($cacheFile) < filemtime($imageFile)))
		{
		 	switch($ext)
 			{
 			case ".jpg":
 				$src = imagecreatefromjpeg($imageFile);
 				break;

 			case ".jpeg":
 				$src = imagecreatefromjpeg($imageFile);
 				break;
				
 			case ".png":
 				$src = imagecreatefrompng($imageFile);
 				break;
 			}
 			
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
						
			$offsetX = ($size - $newWidth) / 2;
			$offsetY = ($size - $newHeight) / 2;
			
			$dst = imagecreatetruecolor($size, $size);
			

			$trans = imagecolorallocatealpha($dst, 255, 255, 255, 127);
			$white = imagecolorallocate($dst, 255, 255, 255);
			
			imagecolortransparent($dst, $trans);
				
			imagefilledrectangle($dst, 0, 0, $size, $size, $white);
			imagefilledrectangle($dst, 0, 0, $size, $size, $trans);
				
			
			imagecopyresampled($dst, $src, $offsetX, $offsetY, 0, 0, $newWidth, $newHeight, $fullWidth, $fullHeight);
			
			if (!file_exists($cacheDir))
			{
				trace("iconize:: creating cachedir", 3);
				mkdir($cacheDir);
			}
			
			if (file_exists($cacheFile))
			{
				// If a previous copy of the file already exists, remove it
				trace("iconize:: unlinking cachefile", 3);
				unlink($cacheFile);
			}
			
			imagepng($dst, $cacheFile);

			imagedestroy($dst);
			imagedestroy($src);
		}

		Fakoli::sendFile($cacheFile);
	}
	
	/**
	 * Returns the URI that can be used to access the specified image at the given size.
	 * @param $image_id the ID of the image
	 * @param $size the size of the major axis in pixels
	 * @return string URI that can be used in an <img> tag to view this image.
	 */
	function thumbnailLink($image_id, $size)
	{
		return "/action/image/thumbnail?image_id=$image_id&size=$size";
	}
	
	function imageLink($image_id)
	{
		return "/action/image/show?image_id=$image_id";
	}
	
	/**
	 * Determines the size of the specified image.
	 * @param $image_id the ID of the image to be queried.
	 * @param $width returns the width of the image in pixels
	 * @param $height returns the height of the image in pixels
	 */
	function getSize($image_id, &$width, &$height)
	{
		$image = new ImageRecord($image_id);
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

	/**
	 * Calculates the width and height of an image after it is scaled to the 
	 * specified size along its major axis.
	 * @param $image_id the ID of the image.
	 * @param $size the required length of the major axis
	 * @param $width returns the width of the scaled image
	 * @param $height returns the height of the scaled image
	 */
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
	
	/**
	 * Event handler to display an image gallery from the specified identifier
	 * @param string $identifier the identifier to match
	 * @param boolean $continue returns whether to continue processing this event
	 */
	static function displayImageGallery($identifier, &$continue)
	{
		try
		{
			$gallery = Query::create(ImageGallery, "WHERE identifier=:i")
							->bind(":i", $identifier)
							->executeSingle();
							
			$page = ComponentPage::findByIdentifier("image_gallery", "WHERE enabled=1");
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
		$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();
		
		$items["Image Galleries"] = $galleries;
		return $items;
	}
	
	
	/**
	 * Upload the image file specified in the given field. 
	 * @param string $field the field name of the image file
	 * @param $image the Image object the field is being uploaded to.
	 * @return boolean true if the upload worked, false if it failed.
	 */
	static function imageUploadHandler($field, $image)
	{
		global $config;
		
		trace("imageUploadHandler() called for $field", 3);
		
		if (!$_FILES[$field]) 
		{
			trace("No upload record for $field", 3);
			return false;
		}
		if ($_FILES[$field]["name"]=="") 
		{
			trace("Upload name is empty", 3);
			return false;
		}
	
		$gallery = $image->Gallery();
		
		$dir = $gallery->getGalleryDirectory();
		if (!file_exists($dir))
		{
			mkdir($dir);
		}
		
		/* Copy across the uploaded file */
	
		trace("Upload Base: {$dir}", 3);
	    
		$filename = $_FILES[$field]['name'];

		$nameParts = explode(".",$filename);		
		$ext = ".".strtolower($nameParts[1]);
		
		if ($ext == ".jpeg")
		{
			$ext = ".jpg";
			$filename = strtolower($nameParts[0]) . $ext;
		}
		
		// $ext = strtolower(substr($filename, -4));
		
		trace("Image format: $ext", 3);
		
		if ($ext != ".jpg" && $ext != ".png") 
		{
			throw new FakoliException("Unsupported image format");
		}
		
		$target = $dir . DIRECTORY_SEPARATOR . $filename;
		
		trace ("Uploading file to $target", 3);
		
		 if (file_exists($target))
		{
			// If a previous copy of the file already exists, remove it
			unlink($target);
		}
		
	  	move_uploaded_file($_FILES[$field]["tmp_name"], $target);
	  	
	  	if ($gallery->fix_orientation && $ext == ".jpg")
	  	{
	  		$rotated = ImageManager::fixOrientation($target);
	  		if ($rotated != $target)
	  		{
	  			$target = $rotated;
	  			$filename = basename($rotated);
	  			$image->set("image_type", "image/png");
	  		}
	  	}
	  	else
	  	{
		  	chmod($target, 0755);
	  	}
	  	
		$image->set("image_file", $filename);  	
	  	return true;
	}
	
	/**
	 * Rotates the image to match the correct orientation when orientation information
	 * is included in the image file. Currently this only works for JPEGs. The target file
	 * is converted to a PNG in the rotation process so that the transformation is lossless.
	 * 
	 * @param string $photo path to the photo that is to be re-oriented
	 * @return the path to the reoriented photo
	 */
	static function fixOrientation($photo)
	{
		trace("Checking Photo Orientation", 3);
		
		$exif = read_exif_data($photo);

		switch($exif['Orientation'])
		{
		case 6:
			$angle = -90;
			break;
			
		case 3:
			$angle = 180;
			break;
			
		case 8:
			$angle = 90;
			
		default:
			
			// No transform
			trace("No rotation required", 3);
			return $photo;
		}

		$source = imagecreatefromjpeg($photo);
		$rotated = imagerotate($source, $angle, 0);

		trace("Rotating $angle degrees", 3);
		
		$photo = preg_replace("/\.jpg\$/", ".png", $photo);
		
		trace("Saving to $photo");
		
		imagepng($rotated, $photo);
		return $photo;
	}
	
	static function imageGalleryTabs($key)
	{
		$tabs = array(
			"Definition"		=>	"image_gallery_form",		
			"Thumbnails"		=>	"images",
			"Detailed List"		=>	"image_list"
		);

		$qs = ($key) ? "gallery_id=$key" : ""; 
		return new TabBar("tabs", $tabs, $qs);
	}
	
	static function getStyles()
	{
		$styles .= "<link href=\"/fakoli/css/tabs.css\" rel=\"stylesheet\"/>\n";
		
		return $styles;
	}	

	static function setDefaults()
	{
		trace("ImageManager::setDefaults", 3);
		
		Settings::setDefaultValue("image", "thumbnail_size", 75, "Number", "");	
	}
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = ImageRecord;
		return $classes;
	}	
	
	static function upgradeComponent($version)
	{
		$mgr = new ImageUpgradeManager();
		$mgr->upgrade($version);
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
		
		trace("Component image is deleting objects dependent on user_id {$user_id}", 3);
	
		$gallery = new ImageGallery();
		$gallery->delete("WHERE owner_id={$user_id}");
	
		return $user;
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("image_galleries", "Image Galleries &amp; Image Records (files need to be transferred manually)", new ImageSerializationHandler());
		return true;
	}
}

class ImageGalleryScanManager
{
	var $gallery;
	var $indexedImages = array();
	
	function __construct($gallery)
	{
		$this->gallery = $gallery;
		
		$images = $this->gallery->Images();
		if(count($images))
		{
			$this->indexedImages = reindexList($images, "image_file");
		}
		else
		{
			$this->indexedImages = array();
		}			
	}
	
	function rescan()
	{
		global $user;
	
		$base = sanitizePath($this->gallery->getGalleryDirectory());
	
		$files = scandir($base);
		foreach($files as $file)
		{
			if ($file == "." || $file == "..") continue;
	
			if ($this->indexedImages[$file]) continue;
	
			$image = new ImageRecord();
			$image->title = $file;
			$image->image_file = $file;
			$image->ALT_tag = "";
			$image->published = true;
			$image->gallery_id = $this->gallery->gallery_id;
			$image->owner_id= $user->get($user->getPrimaryKey());
			$image->save();
		}
	}
}