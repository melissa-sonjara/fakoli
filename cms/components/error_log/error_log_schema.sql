-- Fakoli Error Log Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE error_log (
  error_log_id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  code INTEGER UNSIGNED NOT NULL,
  message TEXT NOT NULL,
  file VARCHAR(500) NOT NULL,
  line INTEGER UNSIGNED NOT NULL,
  details TEXT NOT NULL,
  user_id INTEGER UNSIGNED NOT NULL,
  uri VARCHAR(1000) NOT NULL DEFAULT '',
  session VARCHAR(200) NOT NULL DEFAULT '',
  timestamp DATETIME NOT NULL,
  PRIMARY KEY (error_log_id)
)
ENGINE = InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE error_log ADD COLUMN referer VARCHAR(1000) NOT NULL DEFAULT '';

-- END Version 1.1

-- START Version 1.2

ALTER TABLE fakoli_default.error_log
 ADD INDEX error_log_session_idx(user_id, session),
 ADD INDEX error_log_timestamp_idx(timestamp, user_id);

-- END Version 1.2