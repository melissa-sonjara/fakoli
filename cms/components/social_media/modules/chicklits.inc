<? 
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

$url = "http://{$_SERVER['SERVER_NAME']}{$_SERVER['REQUEST_URI']}"; 
?>
 <a target="new_page" href="http://www.facebook.com/share.php?u=<?echo $url ?>"><img
 src="/fakoli/social_media/images/facebook_share_icon.gif"
 alt="Share this page on Facebook" width="16" align="middle" border="0"
 height="16"></a>
 <a  target="new_page" href="http://twitter.com/home?status=<?php echo $head_title?>: <?php echo $url ?>"> <img
 src="/fakoli/social_media/images/twitter.png"
 alt="Share this page on Twitter" width="16" align="middle" border="0"
 height="16"></a>  
 <a target="new_page" href="http://digg.com/submit?url=<?echo $url ?>">
 <img src="/fakoli/social_media/images/digg.png" alt="Digg this page" width="16"
 align="middle" border="0" height="14"></a>
 <a target="new_page" href="http://www.stumbleupon.com/submit?url=<?echo $url ?>"><img
 src="/fakoli/social_media/images/stumbleupon_16x16.gif"
 alt="Share this page on StumbleUpon" width="16" align="middle"
 border="0" height="16"></a>
 <a target="new_page" href="http://del.icio.us/post?v=4%26noui%26jump=close%26url=<?echo $url ?>"><img
 src="/fakoli/social_media/images/delicious.png" alt="Post This Page to Del.icio.us"
 width="16" align="middle" border="0" height="16"></a>
 <a title="Save to Reddit" target="new_page" href="http://reddit.com/submit?url=<?echo $url ?>;"><img
 src="/fakoli/social_media/images/reddit.png" alt="Save this page to Reddit"
 width="16" align="middle" border="0" height="18"></a>
 <a target="new_page" href="http://myweb.yahoo.com/myresults/bookmarklet?u=<?echo $url ?>&ei=UTF"><img
 src="/fakoli/social_media/images/yahoomyweb.png" alt="Save this page to Yahoo MyWeb"
 width="16" align="middle" border="0" height="9"></a>
 <a target="new_page" href="http://www.newsvine.com/_tools/seed%26save?url=<?echo $url ?>"><img
 src="/fakoli/social_media/images/newsvine.gif" alt="Save this page to Newsvine"
 width="16" align="middle" border="0" height="16"></a>
 <a target="new_page" href="http://www.google.com/bookmarks/mark?op=edit%26bkmk=<?echo $url ?>"><img
 src="/fakoli/social_media/images/google.png" alt="Save this page to Google Bookmarks"
 width="16" align="middle" border="0" height="16"></a>
 <a target="new_page" href="http://www.mixx.com/submit?page_url=<?echo $url ?>"><img
 src="/fakoli/social_media/images/mixx.png" alt="Save this page to Mixx" width="16"
 align="middle" border="0" height="14"></a>
 <a target="new_page" href="http://technorati.com/faves?add=<?echo $url ?>"><img
 src="/fakoli/social_media/images/technorati.png" alt="Save this page to Technorati"
 width="16" align="middle" border="0" height="16"></a>
