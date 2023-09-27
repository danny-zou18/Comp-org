"""Migration for a given Submitty course database."""

import os
import grp
import pwd
from pathlib import Path
import shutil

def up(config, database, semester, course):

    course_dir = Path(config.submitty['submitty_data_dir'], 'courses', semester, course)
    generated_output_dir = Path(course_dir, 'generated_output')

    # get course group
    stat_info = os.stat(str(course_dir))
    course_group_id = stat_info.st_gid
    uid = stat_info.st_uid
    course_group = grp.getgrgid(course_group_id)[0]
    user = pwd.getpwuid(uid)[0]

    if not os.path.exists(generated_output_dir):
        os.mkdir(generated_output_dir)
        shutil.chown(generated_output_dir, user=user, group=course_group)
        os.system(f"chmod u+rwx {0}"{str(generated_output_dir}))
        os.system(f"chmod g+rwx {0}"{str(generated_output_dir}))
        os.system(f"chmod g+s {0}"{str(generated_output_dir}))
        os.system(f"chmod o-rwx {0}"{str(generated_output_dir}))


def down(config, database, semester, course):
    pass