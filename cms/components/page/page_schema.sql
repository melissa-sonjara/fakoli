-- Fakoli Page Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE page (
  page_id int(10) unsigned NOT NULL AUTO_INCREMENT,
  page_title varchar(200) DEFAULT NULL,
  description longtext,
  author varchar(45) NOT NULL,
  created_date date NOT NULL,
  edited_date date NOT NULL,
  meta_tag_description varchar(200) DEFAULT NULL,
  meta_tag_keyword varchar(200) DEFAULT NULL,
  role varchar(100) NOT NULL DEFAULT '',
  published tinyint(3) unsigned NOT NULL,
  template varchar(100) DEFAULT NULL,
  identifier varchar(45) NOT NULL,
  php_code_file varchar(100) NOT NULL DEFAULT '',
  site_id int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (page_id)
) ENGINE=InnoDB;

/*!40000 ALTER TABLE page DISABLE KEYS */;
INSERT INTO page (page_id,page_title,description,author,created_date,edited_date,meta_tag_description,meta_tag_keyword,role,published,template,identifier,php_code_file,site_id) VALUES 
 (1,'Welcome to Fakoli','<p>Congratulations, you have launched your new Fakoli instance.</p>\r\n<p>The next most common steps are (links open up the relevant pages on Fakoli.org documentation):</p>\r\n<ol>\r\n<li><a href=\"http://www.fakoli.org/cms?article_id=39\" target=\"_blank\">Access the site administration </a></li>\r\n<li><a href=\"http://www.fakoli.org/cms?article_id=61\" target=\"_blank\">Create new users</a></li>\r\n<li><a href=\"http://www.fakoli.org/cms_guide?article_id=9\" target=\"_blank\">Change the template (banner, colors, layout, images)</a></li>\r\n<li><a href=\"http://www.fakoli.org/cms?article_id=43\" target=\"_blank\">Change content (add pages, menu items, blogs,&nbsp; document libraries, etc)</a></li>\r\n<li><a href=\"http://www.fakoli.org/cms_guide?article_id=11\" target=\"_blank\">Add custom code components </a></li>\r\n</ol>\r\n<p>To get to the Admin section of your Fakoli site, go to <a href=\"/admin/\" target=\"_self\">/admin</a> or click on the menu button above called ADMIN.</p>\r\n<p>Default username is: admin</p>\r\n<p>Default password is: admin</p>\r\n<p>CHANGE THIS IMMEDIATELY AFTER LOGGING IN.</p>','','2013-09-21','2013-09-21','','','',1,'default.tpl','index','',1),
 (2,'Second Page','<p>This page is an example of a second page in Fakoli.</p>','','2013-09-21','2013-09-21','','','',1,'default.tpl','second_page','',1);
/*!40000 ALTER TABLE page ENABLE KEYS */;

CREATE TABLE page_module_xref (
  join_id int(10) unsigned NOT NULL AUTO_INCREMENT,
  page_id int(10) unsigned NOT NULL,
  module_id int(10) unsigned NOT NULL,
  position varchar(45) NOT NULL,
  sort_order int(10) unsigned NOT NULL,
  PRIMARY KEY (join_id)
) ENGINE=InnoDB;

-- END Version 1.0
