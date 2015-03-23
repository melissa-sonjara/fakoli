-- Component Schema for OpenData
--
-- Author: Siobhan Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.1
CREATE TABLE open_data (
  open_data_id INT NOT NULL AUTO_INCREMENT,
  class VARCHAR(500) NULL,
  field VARCHAR(500) NULL,
  open_data_level VARCHAR(500) NULL,
  redacted_field INT NOT NULL DEFAULT 0,
  redacted_record INT NOT NULL DEFAULT 0,
  private INT NOT NULL DEFAULT 0,
  protected INT NOT NULL DEFAULT 0,
  source VARCHAR(500) NULL,
  data_standard VARCHAR(500) NULL,
  description LONGTEXT NULL,
  PRIMARY KEY (open_data_id)
);

--END Version 1.1

--START version 1.2

ALTER TABLE open_data ADD COLUMN ignore TINYINT(1) NULL DEFAULT 0 ;
ALTER TABLE open_data ADD COLUMN field_type VARCHAR(500) ;
--END Version 1.2
