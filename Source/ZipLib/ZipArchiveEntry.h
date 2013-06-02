#pragma once
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>

#include "ZipLocalFileHeader.h"
#include "ZipCentralDirectoryFileHeader.h"

#include "streams/infstream.h"
#include "streams/substream.h"
#include "EnumTools.h"

class ZipArchive;

/**
 * \brief Represents a compressed file within a zip archive.
 */
class ZipArchiveEntry
{
  friend class ZipFile;
  friend class ZipArchive;
  friend class ZipCrypto;

  public:
    /**
     * \brief Values that represent the method of compression.
     */
    enum class CompressionMethod : uint16_t
    {
      Deflate = 8,
      Stored = 0
    };

    /**
     * \brief Values that represent the level of the compression.
     */
    enum class CompressionLevel
    {
      Stored = 0,
      BestSpeed = 1,
      Default = 6,
      BestCompression = 9
    };

    /**
     * \brief Values that represent the way the zip entry will be compressed.
     */
    enum class CompressionMode
    {
      Immediate,
      Deferred
    };

    /**
     * \brief Values that represent the MS-DOS file attributes.
     */
    enum class Attributes : uint32_t
    {
      None = 0,
      ReadOnly = 1,
      Hidden = 2,
      System = 4,
      Directory = 16,
      Archive = 32,
      Device = 64,
      Normal = 128,
      Temporary = 256,
      SparseFile = 512,
      ReparsePoint = 1024,
      Compressed = 2048,
    };

    MARK_AS_TYPED_ENUMFLAGS_FRIEND(CompressionMethod);
    MARK_AS_TYPED_ENUMFLAGS_FRIEND(CompressionLevel);
    MARK_AS_TYPED_ENUMFLAGS_FRIEND(Attributes);
    MARK_AS_TYPED_ENUMFLAGS_FRIEND(CompressionMode);

    /**
     * \brief Destructor.
     */
    ~ZipArchiveEntry();

    /**
     * \brief Gets full path of the entry.
     *
     * \return  The full name with the path.
     */
    const std::string& GetFullName() const;

    /**
     * \brief Sets full name with the path of the entry.
     *
     * \param fullName The full name with the path.
     */
    void SetFullName(const std::string& fullName);

    /**
     * \brief Gets only the file name of the entry (without path).
     *
     * \return  The file name.
     */
    std::string GetName() const;

    /**
     * \brief Sets only a file name of the entry.
     *        If the file is located within some folder, the path is kept.
     *
     * \param name  The file name.
     */
    void SetName(const std::string& name);

    /**
     * \brief Gets the comment of this zip entry.
     *
     * \return  The comment.
     */
    const std::string& GetComment() const;

    /**
     * \brief Sets a comment of this zip entry.
     *
     * \param comment The comment.
     */
    void SetComment(const std::string& comment);

    /**
     * \brief Gets the time the file was last modified.
     *
     * \return  The last write time.
     */
    time_t GetLastWriteTime() const;

    /**
     * \brief Sets the time the file was last modified.
     *
     * \param modTime Time of the modifier.
     */
    void SetLastWriteTime(time_t modTime);

    /**
     * \brief Gets the file attributes of this zip entry.
     *
     * \return  The file attributes.
     */
    Attributes GetAttributes() const;

    /**
     * \brief Gets the compression method.
     *
     * \return  The compression method.
     */
    CompressionMethod GetCompressionMethod() const;

    /**
     * \brief Sets the file attributes of this zip entry.
     *
     * \param value The file attributes.
     */
    void SetAttributes(Attributes value);

    /**
     * \brief Query if this entry is password protected.
     *
     * \return  true if password protected, false if not.
     */
    bool IsPasswordProtected() const;

    /**
     * \brief Gets the password of the zip entry. If the password is empty string, the password is not set.
     *
     * \return  The password.
     */
    const std::string& GetPassword() const;

    /**
     * \brief Sets a password of the zip entry. If the password is empty string, the password is not set.
     *        Use before GetDecompressionStream or SetCompressionStream.
     *
     * \param password  The password.
     */
    void SetPassword(const std::string& password);

    /**
     * \brief Gets CRC 32 of the file.
     *
     * \return  The CRC 32.
     */
    uint32_t GetCrc32() const;

    /**
     * \brief Gets the size of the uncompressed data.
     *
     * \return  The size.
     */
    size_t GetSize() const;

    /**
     * \brief Gets the size of compressed data.
     *
     * \return  The compressed size.
     */
    size_t GetCompressedSize() const;

    /**
     * \brief Determine if we can extract the entry.
     *        It depends on which version was the zip archive created with.
     *
     * \return  true if we can extract, false if not.
     */
    bool CanExtract() const;

    /**
     * \brief Query if this entry is a directory.
     *
     * \return  true if directory, false if not.
     */
    bool IsDirectory() const;

    /**
     * \brief Query if this object is using data descriptor.
     *        Data descriptor is small chunk of information written after the compressed data.
     *        It's most useful when encrypting a zip entry.
     *        When it is not using, the CRC32 value is required before
     *        encryption of the file data begins. In this case there is no way
     *        around it: must read the stream in its entirety to compute the
     *        actual CRC32 before proceeding.
     *
     * \return  true if using data descriptor, false if not.
     */
    bool IsUsingDataDescriptor() const;

    /**
     * \brief Use data descriptor.
     *        Data descriptor is small chunk of information written after the compressed data.
     *        It's most useful when encrypting a zip entry.
     *        When it is not using, the CRC32 value is required before
     *        encryption of the file data begins. In this case there is no way
     *        around it: must read the stream in its entirety to compute the
     *        actual CRC32 before proceeding.
     * \param use (Optional) If true, use the data descriptor, false to not use.
     */
    void UseDataDescriptor(bool use = true);

    /**
     * \brief Gets raw stream of the compressed data.
     *
     * \return  null if it fails, else the stream of raw data.
     */
    std::istream* GetRawStream();

    /**
     * \brief Gets decompression stream.
     *        If the file is encrypted and correct password is not provided, it returns nullptr.
     *
     * \return  null if it fails, else the decompression stream.
     */
    std::istream* GetDecompressionStream();

    /**
     * \brief Query if the GetRawStream method has been already called.
     *
     * \return  true if the raw stream is opened, false if not.
     */
    bool IsRawStreamOpened() const;

    /**
     * \brief Query if the GetDecompressionStream method has been already called.
     *
     * \return  true if the decompression stream is opened, false if not.
     */
    bool IsDecompressionStreamOpened() const;

    /**
     * \brief Closes the raw stream, opened by GetRawStream.
     */
    void CloseRawStream();
    
    /**
     * \brief Closes the decompression stream, opened by GetDecompressionStream.
     */
    void CloseDecompressionStream();

    /**
     * \brief Sets the input stream to fetch the data to compress from.
     *
     * \param stream            The input stream to compress.
     * \param level             (Optional) The level of compression.
     * \param method            (Optional) The method of compression.
     * \param mode              (Optional) The mode of compression.
     *                          If deferred mode is chosen, the data are compressed when the zip archive is about to be written.
     *                          The stream instance must exist when the ZipArchive::WriteToStream method is called.
     *                          The advantage of deferred compression mode is the compressed data needs not to be loaded
     *                          into the memory, because they are streamed into the final output stream.
     *                          
     *                          If immediate mode is chosen, the data are compressed immediately into the memory buffer.
     *                          It is not recommended to use this method for large files.
     *                          The advantage of immediate mode is the input stream can be destroyed (i.e. by scope)
     *                          even before the ZipArchive::WriteToStream method is called.
     *
     * \return  true if it succeeds, false if it fails.
     */
    bool SetCompressionStream(std::istream&     stream,
                              CompressionLevel  level  = CompressionLevel::Default,
                              CompressionMethod method = CompressionMethod::Deflate,
                              CompressionMode   mode   = CompressionMode::Deferred);

    /**
     * \brief Sets compression stream to be null and unsets the password. The entry would contain no data with zero size.
     */
    void UnsetCompressionStream();

    /**
     * \brief Removes this entry from the ZipArchive.
     */
    void Remove();

  private:
    static const uint16_t DEFAULT_VERSION_MADEBY = 20;

    enum class BitFlag : uint16_t
    {
      None = 0,
      Encrypted = 1,
      DataDescriptor = 8,
      UnicodeFileName = 0x800
    };

    enum class ZipVersionNeeded : uint16_t
    {
      Default = 10,
      DeflateAndExplicitDirectory = 20,
      Zip64 = 45
    };

    MARK_AS_TYPED_ENUMFLAGS_FRIEND(BitFlag);
    MARK_AS_TYPED_ENUMFLAGS_FRIEND(ZipVersionNeeded);

    ZipArchiveEntry();
    ZipArchiveEntry(const ZipArchiveEntry&);
    ZipArchiveEntry& operator = (ZipArchiveEntry&);

    // static methods
    static ZipArchiveEntry* CreateNew(ZipArchive* zipArchive, const std::string& fullPath);
    static ZipArchiveEntry* CreateExisting(ZipArchive* zipArchive, ZipCentralDirectoryFileHeader& cd);

    static void TimestampToDateTime(time_t dateTime, uint16_t& date, uint16_t& time);
    static time_t DateTimeToTimestamp(uint16_t date, uint16_t time);

    static bool IsValidFilename(const std::string& fullPath);
    static std::string GetFilenameFromPath(const std::string& fullPath);
    static bool IsDirectoryPath(const std::string& fullPath);
    static void CopyStream(std::istream& input, std::ostream& output);

    // methods
    void SetCompressionMethod(CompressionMethod value);

    BitFlag GetGeneralPurposeBitFlag() const;
    void SetGeneralPurposeBitFlag(BitFlag value, bool set = true);

    ZipVersionNeeded GetVersionToExtract() const;
    void SetVersionToExtract(ZipVersionNeeded value);

    uint16_t GetVersionMadeBy() const;
    void SetVersionMadeBy(uint16_t value);

    int32_t GetOffsetOfLocalHeader() const;
    void SetOffsetOfLocalHeader(int32_t value);

    bool HasCompressionStream() const;

    void FetchLocalFileHeader();
    void CheckFilenameCorrection();
    void FixVersionToExtractAtLeast(ZipVersionNeeded value);

    void SyncLFH_with_CDFH();
    void SyncCDFH_with_LFH();

    std::ios::pos_type GetOffsetOfCompressedData();
    std::ios::pos_type SeekToCompressedData();

    void SerializeLocalFileHeader(std::ostream& stream);
    void SerializeCentralDirectoryFileHeader(std::ostream& stream);

    void UnloadCompressionData();
    void InternalCompressStream(std::istream& inputStream, std::ostream& outputStream);

    // for encryption

    void FigureCrc32();
    uint8_t GetLastByteOfEncryptionHeader();

    //////////////////////////////////////////////////////////////////////////
    ZipArchive* _archive;

    std::istream* _rawStream;
    std::istream* _openedArchive;
    std::istream* _inflateStream;
    std::istream* _zipCryptoStream;

    // TODO: make as flags
    bool _originallyInArchive;
    bool _isNewOrChanged;
    bool _hasLocalFileHeader;

    ZipLocalFileHeader _localFileHeader;
    ZipCentralDirectoryFileHeader _centralDirectoryFileHeader;

    std::ios::pos_type _offsetOfCompressedData;
    std::ios::pos_type _offsetOfSerializedLocalFileHeader;

    std::string _password;

    // internal compression data
    std::istream* _compressionStream;
    CompressionMode _compressionMode;
    CompressionLevel _compressionLevel;
    std::stringstream _compressionImmediateBuffer; // needed for both reading and writing
};
