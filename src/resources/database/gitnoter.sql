DROP TABLE "config";
DROP TABLE "notes";
DROP TABLE "notes_join_tags";
DROP TABLE "tags";
DROP TABLE "categories";
DROP TABLE "notes_join_categories";

CREATE TABLE "config" ("version" TEXT,"repository_dir" TEXT,"repository_url" TEXT);

CREATE TABLE "notes" ("uuid" TEXT NOT NULL,"title" TEXT,"create_date" INTEGER,"update_date" INTEGER,"body" TEXT,"file_dir" TEXT,"file_name" TEXT,PRIMARY KEY ("uuid") );

CREATE TABLE "notes_join_tags" ("notes_uuid" TEXT NOT NULL,"tags_name" INTEGER NOT NULL,"create_date" INTEGER NOT NULL,CONSTRAINT "fk_notes_join_tags_tags_1" FOREIGN KEY ("tags_name") REFERENCES "tags" ("name"),CONSTRAINT "fk_notes_join_tags_notes_1" FOREIGN KEY ("notes_uuid") REFERENCES "notes" ("uuid"));

CREATE TABLE "tags" ("name" TEXT NOT NULL,"create_date" INTEGER NOT NULL,PRIMARY KEY ("name") );

CREATE TABLE "categories" ("name" TEXT NOT NULL,"create_data" INTEGER NOT NULL,"is_default" INTEGER NOT NULL DEFAULT 0,PRIMARY KEY ("name") );

CREATE TABLE "notes_join_categories" ("notes_uuid" TEXT NOT NULL,"categories_name" INTEGER NOT NULL,"create_date" INTEGER NOT NULL,CONSTRAINT "fk_notes_join_categories_notes_1" FOREIGN KEY ("notes_uuid") REFERENCES "notes" ("uuid"),CONSTRAINT "fk_notes_join_categories_categories_1" FOREIGN KEY ("categories_name") REFERENCES "categories" ("name"));