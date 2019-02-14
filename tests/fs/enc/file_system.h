// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef _file_system_h
#define _file_system_h

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <openenclave/enclave.h>
#include <openenclave/internal/fs.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class fs_file_system
{
  public:
    typedef oe_device_t* file_handle;
    typedef oe_device_t* dir_handle;
    typedef struct oe_stat stat_type;
    typedef struct oe_dirent dirent_type;

    fs_file_system(oe_device_t* fs) : _fs(fs)
    {
    }

    file_handle open(const char* pathname, int flags, mode_t mode)
    {
        return (file_handle)_fs->ops.fs->open(_fs, pathname, flags, mode);
    }

    ssize_t write(file_handle file, const void* buf, size_t count)
    {
        return file->ops.fs->base.write((oe_device_t*)file, buf, count);
    }

    ssize_t read(file_handle file, void* buf, size_t count)
    {
        return file->ops.fs->base.read(file, buf, count);
    }

    off_t lseek(file_handle file, off_t offset, int whence)
    {
        return _fs->ops.fs->lseek(file, offset, whence);
    }

    int close(file_handle file)
    {
        return file->ops.fs->base.close((oe_device_t*)file);
    }

    dir_handle opendir(const char* name)
    {
        return _fs->ops.fs->opendir(_fs, name);
    }

    struct oe_dirent* readdir(dir_handle dir)
    {
        return dir->ops.fs->readdir(dir);
    }

    int closedir(dir_handle dir)
    {
        return dir->ops.fs->closedir(dir);
    }

    int unlink(const char* pathname)
    {
        return _fs->ops.fs->unlink(_fs, pathname);
    }

    int link(const char* oldpath, const char* newpath)
    {
        return _fs->ops.fs->link(_fs, oldpath, newpath);
    }

    int rename(const char* oldpath, const char* newpath)
    {
        return _fs->ops.fs->rename(_fs, oldpath, newpath);
    }

    int mkdir(const char* pathname, mode_t mode)
    {
        return _fs->ops.fs->mkdir(_fs, pathname, mode);
    }

    int rmdir(const char* pathname)
    {
        return _fs->ops.fs->rmdir(_fs, pathname);
    }

    int stat(const char* pathname, struct oe_stat* buf)
    {
        return _fs->ops.fs->stat(_fs, pathname, buf);
    }

    int truncate(const char* path, off_t length)
    {
        return _fs->ops.fs->truncate(_fs, path, length);
    }

  private:
    oe_device_t* _fs;
};

class hostfs_file_system : public fs_file_system
{
  public:
    hostfs_file_system() : fs_file_system(oe_fs_get_hostfs())
    {
    }
};

class sgxfs_file_system : public fs_file_system
{
  public:
    sgxfs_file_system() : fs_file_system(oe_fs_get_sgxfs())
    {
    }
};

class oe_fd_file_system
{
  public:
    typedef int file_handle;
    typedef OE_DIR* dir_handle;
    typedef struct oe_stat stat_type;
    typedef struct oe_dirent dirent_type;

    oe_fd_file_system(void)
    {
    }

    file_handle open(const char* pathname, int flags, mode_t mode)
    {
        return (file_handle)oe_open(0, pathname, flags, mode);
    }

    ssize_t write(file_handle file, const void* buf, size_t count)
    {
        return oe_write(file, buf, count);
    }

    ssize_t read(file_handle file, void* buf, size_t count)
    {
        return oe_read(file, buf, count);
    }

    off_t lseek(file_handle file, off_t offset, int whence)
    {
        return oe_lseek(file, offset, whence);
    }

    int close(file_handle file)
    {
        return oe_close(file);
    }

    dir_handle opendir(const char* name)
    {
        return (dir_handle)oe_opendir(0, name);
    }

    struct oe_dirent* readdir(dir_handle dir)
    {
        return oe_readdir(dir);
    }

    int closedir(dir_handle dir)
    {
        return oe_closedir(dir);
    }

    int unlink(const char* pathname)
    {
        return oe_unlink(0, pathname);
    }

    int link(const char* oldpath, const char* newpath)
    {
        return oe_link(0, oldpath, newpath);
    }

    int rename(const char* oldpath, const char* newpath)
    {
        return oe_rename(0, oldpath, newpath);
    }

    int mkdir(const char* pathname, mode_t mode)
    {
        return oe_mkdir(0, pathname, mode);
    }

    int rmdir(const char* pathname)
    {
        return oe_rmdir(0, pathname);
    }

    int stat(const char* pathname, struct oe_stat* buf)
    {
        return oe_stat(0, pathname, buf);
    }

    int truncate(const char* path, off_t length)
    {
        return oe_truncate(0, path, length);
    }

  private:
};

class oe_fd_hostfs_file_system : public oe_fd_file_system
{
  public:
    oe_fd_hostfs_file_system()
    {
        oe_register_hostfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_HOSTFS, NULL, "/", 0) == 0);
    }

    ~oe_fd_hostfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_HOSTFS, "/") == 0);
    }
};

class oe_fd_sgxfs_file_system : public oe_fd_file_system
{
  public:
    oe_fd_sgxfs_file_system()
    {
        oe_register_sgxfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_SGXFS, NULL, "/", 0) == 0);
    }

    ~oe_fd_sgxfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_SGXFS, "/") == 0);
    }
};

class fd_file_system
{
  public:
    typedef int file_handle;
    typedef DIR* dir_handle;
    typedef struct oe_stat stat_type;
    typedef struct dirent dirent_type;

    fd_file_system(void)
    {
    }

    file_handle open(const char* pathname, int flags, mode_t mode)
    {
        return (file_handle)::open(pathname, flags, mode);
    }

    ssize_t write(file_handle file, const void* buf, size_t count)
    {
        return ::write(file, buf, count);
    }

    ssize_t read(file_handle file, void* buf, size_t count)
    {
        return ::read(file, buf, count);
    }

    off_t lseek(file_handle file, off_t offset, int whence)
    {
        return ::lseek(file, offset, whence);
    }

    int close(file_handle file)
    {
        return ::close(file);
    }

    dir_handle opendir(const char* name)
    {
        return (dir_handle)::opendir(name);
    }

    struct dirent* readdir(dir_handle dir)
    {
        return ::readdir(dir);
    }

    int closedir(dir_handle dir)
    {
        return ::closedir(dir);
    }

    int unlink(const char* pathname)
    {
        return ::unlink(pathname);
    }

    int link(const char* oldpath, const char* newpath)
    {
        return ::link(oldpath, newpath);
    }

    int rename(const char* oldpath, const char* newpath)
    {
        return ::rename(oldpath, newpath);
    }

    int mkdir(const char* pathname, mode_t mode)
    {
        return ::mkdir(pathname, mode);
    }

    int rmdir(const char* pathname)
    {
        return ::rmdir(pathname);
    }

    int stat(const char* pathname, struct oe_stat* buf)
    {
        return ::stat(pathname, (struct stat*)buf);
    }

    int truncate(const char* path, off_t length)
    {
        return ::truncate(path, length);
    }

  private:
};

class fd_hostfs_file_system : public fd_file_system
{
  public:
    fd_hostfs_file_system()
    {
        oe_register_hostfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_HOSTFS, NULL, "/", 0) == 0);
    }

    ~fd_hostfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_HOSTFS, "/") == 0);
    }
};

class fd_sgxfs_file_system : public fd_file_system
{
  public:
    fd_sgxfs_file_system()
    {
        oe_register_sgxfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_SGXFS, NULL, "/", 0) == 0);
    }

    ~fd_sgxfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_SGXFS, "/") == 0);
    }
};

class stream_file_system
{
  public:
    typedef FILE* file_handle;
    typedef DIR* dir_handle;
    typedef struct stat stat_type;
    typedef struct dirent dirent_type;

    stream_file_system(void)
    {
    }

    file_handle open(const char* pathname, int flags, mode_t mode)
    {
        FILE* ret = NULL;
        const char* fopen_mode;

        (void)mode;

        switch ((flags & 0x00000003))
        {
            case OE_O_RDONLY:
            {
                fopen_mode = "r";
                break;
            }
            case OE_O_RDWR:
            {
                if (flags & OE_O_CREAT)
                {
                    if (flags & OE_O_TRUNC)
                    {
                        fopen_mode = "w+";
                    }
                    else if (flags & OE_O_APPEND)
                    {
                        fopen_mode = "a+";
                    }
                    else
                    {
                        errno = OE_EINVAL;
                        goto done;
                    }
                }
                else
                {
                    fopen_mode = "r+";
                }
                break;
            }
            case OE_O_WRONLY:
            {
                if (flags & OE_O_CREAT)
                {
                    if (flags & OE_O_TRUNC)
                    {
                        fopen_mode = "w";
                    }
                    else if (flags & OE_O_APPEND)
                    {
                        fopen_mode = "a";
                    }
                    else
                    {
                        errno = OE_EINVAL;
                        goto done;
                    }
                }
                else
                {
                    fopen_mode = "w";
                }
                break;
            }
            default:
            {
                errno = OE_EINVAL;
                goto done;
            }
        }

        ret = ::fopen(pathname, fopen_mode);

    done:
        return ret;
    }

    ssize_t write(file_handle file, const void* buf, size_t count)
    {
        ssize_t ret = -1;

        if (::fwrite(buf, 1, count, file) != count)
        {
            errno = ::ferror(file);
            goto done;
        }

        ret = (ssize_t)count;

    done:
        return ret;
    }

    ssize_t read(file_handle file, void* buf, size_t count)
    {
        ssize_t ret = -1;
        size_t n;

        if ((n = ::fread(buf, 1, count, file)) == 0)
        {
            if (::feof(file))
            {
                errno = ::ferror(file);
                goto done;
            }
        }

        ret = (ssize_t)n;

    done:
        return ret;
    }

    off_t lseek(file_handle file, off_t offset, int whence)
    {
        off_t ret = -1;

        if (::fseek(file, offset, whence) != 0)
        {
            goto done;
        }

        ret = ::ftell(file);

    done:
        return ret;
    }

    int close(file_handle file)
    {
        return ::fclose(file);
    }

    dir_handle opendir(const char* name)
    {
        return (dir_handle)::opendir(name);
    }

    struct dirent* readdir(dir_handle dir)
    {
        return ::readdir(dir);
    }

    int closedir(dir_handle dir)
    {
        return ::closedir(dir);
    }

    int unlink(const char* pathname)
    {
        return ::unlink(pathname);
    }

    int link(const char* oldpath, const char* newpath)
    {
        return ::link(oldpath, newpath);
    }

    int rename(const char* oldpath, const char* newpath)
    {
        return ::rename(oldpath, newpath);
    }

    int mkdir(const char* pathname, mode_t mode)
    {
        return ::mkdir(pathname, mode);
    }

    int rmdir(const char* pathname)
    {
        return ::rmdir(pathname);
    }

    int stat(const char* pathname, struct stat* buf)
    {
        return ::stat(pathname, buf);
    }

    int truncate(const char* path, off_t length)
    {
        return ::truncate(path, length);
    }

  private:
};

class stream_hostfs_file_system : public stream_file_system
{
  public:
    stream_hostfs_file_system()
    {
        oe_register_hostfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_SGXFS, NULL, "/", 0) == 0);
    }

    ~stream_hostfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_SGXFS, "/") == 0);
    }
};

class stream_sgxfs_file_system : public stream_file_system
{
  public:
    stream_sgxfs_file_system()
    {
        oe_register_sgxfs_device();
        OE_TEST(oe_mount(OE_DEVICE_ID_SGXFS, NULL, "/", 0) == 0);
    }

    ~stream_sgxfs_file_system()
    {
        OE_TEST(oe_unmount(OE_DEVICE_ID_SGXFS, "/") == 0);
    }
};

#endif /* _file_system_h */
