from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, OperationFailure, PyMongoError
from typing import Any, Dict, List, Optional


class AnimalShelter:
    """
    CRUD operations for the AAC Animal Shelter MongoDB database.

    CS 499 Milestone Four enhancements:
    1. Improved validation for CRUD operations
    2. Added safer query handling to prevent unintended bulk operations
    3. Added optional projection, sort, and limit support for efficient reads
    4. Improved structure and reusability through helper methods
    5. Improved error handling for maintainability and security-minded design
    """

    DEFAULT_HOST = "nv-desktop-services.apporto.com"
    DEFAULT_PORT = 31306
    DEFAULT_DB = "AAC"
    DEFAULT_COLLECTION = "animals"

    def __init__(
        self,
        username: str,
        password: str,
        host: str = DEFAULT_HOST,
        port: int = DEFAULT_PORT,
        database_name: str = DEFAULT_DB,
        collection_name: str = DEFAULT_COLLECTION,
    ) -> None:
        """
        Initialize MongoDB connection and collection references.
        """
        if not username or not password:
            raise ValueError("Username and password are required.")

        self.username = username
        self.host = host
        self.port = port
        self.database_name = database_name
        self.collection_name = collection_name

        try:
            self.client = MongoClient(
                f"mongodb://{username}:{password}@{host}:{port}/{database_name}?authSource=admin",
                serverSelectionTimeoutMS=5000
            )

            # Force connection test
            self.client.admin.command("ping")

            self.database = self.client[database_name]
            self.collection = self.database[collection_name]

            print("Connected to MongoDB successfully.")

        except ConnectionFailure as exc:
            raise ConnectionError(f"Could not connect to MongoDB: {exc}") from exc
        except OperationFailure as exc:
            raise PermissionError(f"MongoDB authentication failed: {exc}") from exc

    # --------------------------------------------------
    # Internal validation helpers
    # --------------------------------------------------

    def _validate_document(self, data: Dict[str, Any]) -> None:
        """
        Validate document for insert operations.
        """
        if not isinstance(data, dict) or not data:
            raise ValueError("Data must be a non-empty dictionary.")

    def _validate_query(self, query: Dict[str, Any], allow_empty: bool = False) -> None:
        """
        Validate query structure.
        """
        if not isinstance(query, dict):
            raise ValueError("Query must be a dictionary.")
        if not allow_empty and not query:
            raise ValueError("Query must not be empty.")

    def _validate_update_values(self, new_values: Dict[str, Any]) -> None:
        """
        Validate update values.
        """
        if not isinstance(new_values, dict) or not new_values:
            raise ValueError("New values must be a non-empty dictionary.")

    # --------------------------------------------------
    # CRUD operations
    # --------------------------------------------------

    def create(self, data: Dict[str, Any]) -> bool:
        """
        Insert a single document into the collection.

        Returns:
            bool: True if the insert was acknowledged, False otherwise.
        """
        self._validate_document(data)

        try:
            result = self.collection.insert_one(data)
            return result.acknowledged
        except PyMongoError as exc:
            print(f"Insert failed: {exc}")
            return False

    def read(
        self,
        query: Optional[Dict[str, Any]] = None,
        projection: Optional[Dict[str, int]] = None,
        sort: Optional[List[tuple]] = None,
        limit: int = 0
    ) -> List[Dict[str, Any]]:
        """
        Find documents matching a query, with optional projection, sorting, and limiting.

        Args:
            query (dict): MongoDB filter query
            projection (dict): fields to include/exclude
            sort (list): list of tuples for sorting, e.g. [("age_upon_outcome_in_weeks", 1)]
            limit (int): maximum number of records to return

        Returns:
            list: matching documents
        """
        query = query or {}
        self._validate_query(query, allow_empty=True)

        try:
            cursor = self.collection.find(query, projection)

            if sort:
                cursor = cursor.sort(sort)

            if limit > 0:
                cursor = cursor.limit(limit)

            return list(cursor)

        except PyMongoError as exc:
            print(f"Read failed: {exc}")
            return []

    def update(self, query: Dict[str, Any], new_values: Dict[str, Any]) -> int:
        """
        Update documents that match a query.

        Returns:
            int: number of modified documents
        """
        self._validate_query(query, allow_empty=False)
        self._validate_update_values(new_values)

        try:
            result = self.collection.update_many(query, {"$set": new_values})
            return result.modified_count
        except PyMongoError as exc:
            print(f"Update failed: {exc}")
            return 0

    def delete(self, query: Dict[str, Any]) -> int:
        """
        Delete documents that match a query.

        Returns:
            int: number of deleted documents
        """
        self._validate_query(query, allow_empty=False)

        try:
            result = self.collection.delete_many(query)
            return result.deleted_count
        except PyMongoError as exc:
            print(f"Delete failed: {exc}")
            return 0

    # --------------------------------------------------
    # Database-focused enhancement methods
    # --------------------------------------------------

    def read_all(self, limit: int = 0) -> List[Dict[str, Any]]:
        """
        Return all documents, optionally limited.
        """
        return self.read(query={}, limit=limit)

    def read_by_animal_type(self, animal_type: str, limit: int = 0) -> List[Dict[str, Any]]:
        """
        Return documents filtered by animal type.
        """
        if not animal_type:
            raise ValueError("Animal type is required.")

        return self.read(query={"animal_type": animal_type}, limit=limit)

    def read_for_dashboard(self, query: Dict[str, Any]) -> List[Dict[str, Any]]:
        """
        Specialized dashboard read with projection for efficiency.

        Returns only fields relevant to dashboard display.
        """
        projection = {
            "_id": 0,
            "name": 1,
            "animal_type": 1,
            "breed": 1,
            "color": 1,
            "date_of_birth": 1,
            "outcome_type": 1,
            "sex_upon_outcome": 1,
            "age_upon_outcome_in_weeks": 1,
            "location_lat": 1,
            "location_long": 1
        }

        return self.read(
            query=query,
            projection=projection,
            sort=[("age_upon_outcome_in_weeks", 1)]
        )