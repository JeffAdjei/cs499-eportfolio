# AnimalShelter.py

from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, OperationFailure

class AnimalShelter:
    """CRUD operations for the AAC Animal Shelter MongoDB database."""

    def __init__(self, username, password):
        HOST = "nv-desktop-services.apporto.com"
        PORT = 31306
        DB = "AAC"
        COL = "animals"

        try:
            # Connect to MongoDB with authentication
            self.client = MongoClient(
                f"mongodb://{username}:{password}@{HOST}:{PORT}/{DB}?authSource=admin"
            )
            self.database = self.client[DB]
            self.collection = self.database[COL]
            print("✅ Connected to MongoDB.")
        except ConnectionFailure as e:
            print(f"❌ Could not connect to MongoDB: {e}")

    def create(self, data):
        """Insert a single document into the collection."""
        if isinstance(data, dict) and data:
            try:
                result = self.collection.insert_one(data)
                return result.acknowledged
            except Exception as e:
                print(f"Insert failed: {e}")
                return False
        else:
            raise ValueError("Data must be a non-empty dictionary.")

    def read(self, query):
        """Find documents matching a query."""
        if isinstance(query, dict) and query is not None:
            try:
                return list(self.collection.find(query))
            except Exception as e:
                print(f"Read failed: {e}")
                return []
        else:
            raise ValueError("Query must be a non-empty dictionary.")

    def update(self, query, new_values):
        """Update documents that match a query."""
        if isinstance(query, dict) and isinstance(new_values, dict):
            try:
                result = self.collection.update_many(query, {"$set": new_values})
                return result.modified_count
            except Exception as e:
                print(f"Update failed: {e}")
                return 0
        else:
            raise ValueError("Query and new values must be dictionaries.")

    def delete(self, query):
        """Delete documents that match a query."""
        if isinstance(query, dict) and query:
            try:
                result = self.collection.delete_many(query)
                return result.deleted_count
            except Exception as e:
                print(f"Delete failed: {e}")
                return 0
        else:
            raise ValueError("Query must be a non-empty dictionary.")