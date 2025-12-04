FROM node:24.9

WORKDIR app/

COPY frontend/package*.json /app

RUN npm ci

COPY frontend/ /app

CMD ["npm", "run", "dev", "--", "--host", "0.0.0.0"]