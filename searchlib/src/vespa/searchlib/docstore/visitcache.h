// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "idocumentstore.h"
#include <vespa/vespalib/stllike/cache.h>

namespace search {
namespace docstore {

class KeySet {
public:
    KeySet(const IDocumentStore::LidVector &keys);
    uint32_t hash() const { return _keys.empty() ? 0 : _keys[0]; }
    bool operator==(const KeySet &rhs) const { return _keys == rhs._keys; }
    bool operator<(const KeySet &rhs) const { return _keys < rhs._keys; }
    bool contains(const KeySet &rhs) const;
private:
    IDocumentStore::LidVector _keys;
};

class BlobSet {
public:
    class LidPosition {
    public:
        LidPosition(uint32_t lid, uint32_t position, uint32_t size) : _lid(lid), _position(position), _size(size) { }
        uint32_t lid() const { return _lid; }
        uint32_t position() const { return _position; }
        uint32_t size() const { return _size; }
    private:
        uint32_t _lid;
        uint32_t _position;
        uint32_t _size;
    };

    typedef std::vector<LidPosition> Positions;
    void append(uint32_t lid, vespalib::ConstBufferRef blob);
    void remove(uint32_t lid);
    vespalib::ConstBufferRef get(uint32_t lid);
    const vespalib::DataBuffer &getBuffer() const { return _buffer; }
    static Positions getPositions(BlobSet &&blobs) { return std::move(blobs._positions); }
private:
    Positions _positions;
    vespalib::DataBuffer _buffer;
};

class CompressedBlobSet {
public:
    CompressedBlobSet(BlobSet &&uncompressed);
    BlobSet getBlobSet() const;
private:
    BlobSet::Positions   _positions;
    vespalib::DataBuffer _buffer;
};

class VisitCache {
public:
    using Keys=IDocumentStore::LidVector;

    VisitCache(IDataStore &store, size_t cacheSize, const document::CompressionConfig &compression);

    CompressedBlobSet read(const Keys & keys) const;
    void remove(uint32_t key);

private:
    class BackingStore {
    public:
        BackingStore(IDataStore &store, const document::CompressionConfig &compression) :
            _backingStore(store),
            _compression(compression)
        { }
        bool read(const KeySet &key, CompressedBlobSet &blobs) const;
        void write(const KeySet &, const CompressedBlobSet &) { }
        void erase(const KeySet &) { }
        const document::CompressionConfig &getCompression(void) const { return _compression; }
    private:
        IDataStore &_backingStore;
        const document::CompressionConfig &_compression;
    };

    typedef vespalib::CacheParam<vespalib::LruParam<KeySet, BlobSet>,
        BackingStore,
        vespalib::zero<KeySet>,
        vespalib::size<BlobSet> > CacheParams;
    typedef vespalib::cache<CacheParams> Cache;

    BackingStore _store;
    Cache        _cache;
};

}
}
