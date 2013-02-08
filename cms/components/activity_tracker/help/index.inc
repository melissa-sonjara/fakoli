<h2>Activity Tracking and Feedback</h2>
<p>The Activity Tracker component allows you to track user activity on your site
for debugging and troubleshooting purposes. It also provides a simple 
user feedback feature that allows users to report issues or provide usability
feedback directly within your application.</p>

<h3>Activity Tracking</h3>
<p>When activity tracking is turned on (via the Activity Tracker tab in Application Settings)
all user requests to the site are logged to the database, with the time, user account and
session ID. This increases overhead slightly as well as increasing database storage, so it is
not a recommended configuration for heavily loaded production servers. However it can be 
an invaluable tool well testing your application, as it provides a complete click-track of
each user's path through the system.</p>
<p>The Activity Report can be accessed from the <b>User Management</b> administration section,
and can be filtered by authenticated and unauthenticated users (even unauthenticated users
are provided a session ID, so we can track their paths through the system individually).</p>
<p>By clicking on a Session ID from the list you are taken to a report of the activity 
under that session, showing the user's detailed click-track.</p>

<h3>Feedback</h3>
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
<p>It is recommended that you activate activity tracking when making use of the feedback feature.
This will enable you to determine what a user had been doing before posting the feedback message,
which can provide useful usability information in cases of user confusion.</p>


