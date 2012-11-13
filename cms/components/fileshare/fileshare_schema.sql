-- Fakoli Fileshare Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `fileshare_user_xref` (
  `fileshare_user_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_library_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`fileshare_user_xref_id`)
) ENGINE=InnoDB COMMENT='Shares document library acces with users';


-- END Version 1.0
