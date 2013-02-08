<h2>Feedback</h2>
<p>The feedback feature allows users to provide feedback from a comment area that appears
by default as an overlay in the bottom right hand corner of each page. This feature is designed
to help with gathering user feedback and bug reports during usability testing. To enable the
feature, do the following:
<ol>
<li>Go to Modules under <b>Site Content</b> and add a code module mapping to the feedback
module in the activity_tracker component. This module should be global and have the global
position of 'feedback' (you can change this name if you prefer something else).</li>
<li>In your template add {position:feedback} within the body of your HTML.</li>
<li>Go to Application Settings, select the <b>Activity Tracker</b> tab and check the
<b>Enable Feedback</b> checkbox.</li>
</ol>
<p>Once you have enabled the module, users will be able to post feedback directly from each
page on your site, and you will be able to read through these feedback messages in <b>Feedback</b>
section under <b>User Management</b>.</p>

<p>For more information see <a href='page?src=activity_tracker&page=index'>Activity Tracking and Feedback</a>.</p>
